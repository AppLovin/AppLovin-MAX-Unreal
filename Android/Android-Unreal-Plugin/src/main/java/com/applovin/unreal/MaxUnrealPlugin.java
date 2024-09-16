package com.applovin.unreal;

import android.app.Activity;
import android.content.Context;
import android.graphics.Color;
import android.graphics.Rect;
import android.net.Uri;
import android.text.TextUtils;
import android.util.Log;
import android.view.Gravity;
import android.view.View;
import android.view.ViewGroup;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;

import com.applovin.impl.sdk.utils.JsonUtils;
import com.applovin.impl.sdk.utils.StringUtils;
import com.applovin.mediation.MaxAd;
import com.applovin.mediation.MaxAdFormat;
import com.applovin.mediation.MaxAdListener;
import com.applovin.mediation.MaxAdRevenueListener;
import com.applovin.mediation.MaxAdViewAdListener;
import com.applovin.mediation.MaxError;
import com.applovin.mediation.MaxReward;
import com.applovin.mediation.MaxRewardedAdListener;
import com.applovin.mediation.ads.MaxAdView;
import com.applovin.mediation.ads.MaxInterstitialAd;
import com.applovin.mediation.ads.MaxRewardedAd;
import com.applovin.sdk.AppLovinCmpError;
import com.applovin.sdk.AppLovinCmpService;
import com.applovin.sdk.AppLovinMediationProvider;
import com.applovin.sdk.AppLovinPrivacySettings;
import com.applovin.sdk.AppLovinSdk;
import com.applovin.sdk.AppLovinSdkConfiguration;
import com.applovin.sdk.AppLovinSdkConfiguration.ConsentFlowUserGeography;
import com.applovin.sdk.AppLovinSdkInitializationConfiguration;
import com.applovin.sdk.AppLovinSdkUtils;

import org.json.JSONObject;

import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.atomic.AtomicBoolean;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import lombok.val;
import lombok.var;

/**
 * Created by Ritam Sarmah on May 5 2021
 */
@SuppressWarnings("unused")
public class MaxUnrealPlugin
        implements MaxAdListener, MaxAdViewAdListener, MaxRewardedAdListener, MaxAdRevenueListener, AppLovinCmpService.OnCompletedListener
{
    private static final String TAG     = "MaxUnrealPlugin";
    private static final String SDK_TAG = "AppLovinSdk";

    private final AppLovinSdk   sdk;
    private final AtomicBoolean pluginInitialized = new AtomicBoolean();
    private final AtomicBoolean sdkInitialized    = new AtomicBoolean();

    @Nullable
    private AppLovinSdkConfiguration sdkConfiguration;

    // Store these values if pub sets before initializing
    @Nullable
    private List<String> testDeviceAdvertisingIdsToSet;

    // Fullscreen Ad Fields
    private final Map<String, MaxInterstitialAd> interstitials = new HashMap<>( 2 );
    private final Map<String, MaxRewardedAd>     rewardedAds   = new HashMap<>( 2 );

    // Banner Fields
    private final Map<String, MaxAdView>   adViews                    = new HashMap<>( 2 );
    private final Map<String, MaxAdFormat> adViewAdFormats            = new HashMap<>( 2 );
    private final Map<String, MaxAdFormat> verticalAdViewFormats      = new HashMap<>( 2 );
    private final Map<String, String>      adViewPositions            = new HashMap<>( 2 );
    private final List<String>             adUnitIdsToShowAfterCreate = new ArrayList<>( 2 );

    private final WeakReference<Activity> gameActivityRef;

    @Nullable
    private EventListener eventListener;

    private Activity getGameActivity() throws IllegalStateException
    {
        val gameActivity = gameActivityRef.get();
        if ( gameActivity == null ) throw new IllegalStateException( "No Game Activity found" );

        return gameActivityRef.get();
    }

    /**
     * Interface for listening to events dispatched from the plugin. Used to forward events back to Unreal GameActivity.
     */
    public interface EventListener
    {
        void onReceivedEvent(final String name, final String body);
    }

    // region Initialization
    public MaxUnrealPlugin(final Activity activity)
    {
        gameActivityRef = new WeakReference<>( activity );
        sdk = AppLovinSdk.getInstance( activity.getApplicationContext() );
    }

    public void initialize(final String pluginVersion, final String sdkKey, final EventListener listener)
    {
        val currentActivity = getGameActivity();
        val context = currentActivity.getApplicationContext();

        // Guard against running init logic multiple times
        if ( pluginInitialized.compareAndSet( false, true ) )
        {
            sendInitializationEvent( context );
            return;
        }

        d( "Initializing AppLovin MAX Unreal v" + pluginVersion + "..." );

        eventListener = listener;

        if ( TextUtils.isEmpty( sdkKey ) )
        {
            throw new IllegalStateException( "Unable to initialize AppLovin MAX Unreal Plugin - no SDK key provided" );
        }

        // Create initialization configuration
        val initConfigurationBuilder = AppLovinSdkInitializationConfiguration.builder( sdkKey, context )
                .setMediationProvider( AppLovinMediationProvider.MAX )
                .setPluginVersion( "Unreal-" + pluginVersion );

        if ( testDeviceAdvertisingIdsToSet != null )
        {
            initConfigurationBuilder.setTestDeviceAdvertisingIds( testDeviceAdvertisingIdsToSet );
        }

        val initConfiguration = initConfigurationBuilder.build();

        // Initialize SDK
        sdk.initialize( initConfiguration, sdkConfiguration -> {

            this.sdkConfiguration = sdkConfiguration;
            sdkInitialized.set( true );

            // Enable orientation change listener, so that the ad view positions can be updated when the device is rotated.
            val rootView = getGameActivity().getWindow().getDecorView().getRootView();
            rootView.addOnLayoutChangeListener( (view, left, top, right, bottom, oldLeft, oldTop, oldRight, oldBottom) -> {
                val viewBoundsChanged = left != oldLeft || right != oldRight || bottom != oldBottom || top != oldTop;
                if ( !viewBoundsChanged ) return;

                for ( val adUnitFormats : verticalAdViewFormats.entrySet() )
                {
                    positionAdView( adUnitFormats.getKey(), adUnitFormats.getValue() );
                }
            } );

            sendInitializationEvent( context );
        } );
    }

    private void sendInitializationEvent(final Context context)
    {
        val params = new JSONObject();

        if ( sdkConfiguration != null )
        {
            JsonUtils.putInt( params, "consentFlowUserGeography", sdkConfiguration.getConsentFlowUserGeography().ordinal() );
            JsonUtils.putString( params, "countryCode", sdkConfiguration.getCountryCode() );
        }

        JsonUtils.putBoolean( params, "hasUserConsent", AppLovinPrivacySettings.hasUserConsent( context ) );
        JsonUtils.putBoolean( params, "isDoNotSell", AppLovinPrivacySettings.isDoNotSell( context ) );
        JsonUtils.putBoolean( params, "isTablet", AppLovinSdkUtils.isTablet( context ) );

        sendUnrealEvent( "OnSdkInitializedEvent", params );
    }

    public boolean isInitialized()
    {
        return pluginInitialized.get() && sdkInitialized.get();
    }
    // endregion

    // region Privacy
    public void setHasUserConsent(final boolean hasUserConsent)
    {
        AppLovinPrivacySettings.setHasUserConsent( hasUserConsent, getGameActivity() );
    }

    public boolean hasUserConsent()
    {
        return AppLovinPrivacySettings.hasUserConsent( getGameActivity() );
    }

    public void setDoNotSell(final boolean doNotSell)
    {
        AppLovinPrivacySettings.setDoNotSell( doNotSell, getGameActivity() );
    }

    public boolean isDoNotSell()
    {
        return AppLovinPrivacySettings.isDoNotSell( getGameActivity() );
    }
    // endregion

    // region Terms and Privacy Policy Flow
    public void setTermsAndPrivacyPolicyFlowEnabled(final boolean enabled)
    {
        sdk.getSettings().getTermsAndPrivacyPolicyFlowSettings().setEnabled( enabled );
    }

    public void setPrivacyPolicyUrl(final String uriString)
    {
        val uri = Uri.parse( uriString );
        sdk.getSettings().getTermsAndPrivacyPolicyFlowSettings().setPrivacyPolicyUri( uri );
    }

    public void setTermsOfServiceUrl(final String uriString)
    {
        val uri = Uri.parse( uriString );
        sdk.getSettings().getTermsAndPrivacyPolicyFlowSettings().setTermsOfServiceUri( uri );
    }

    public void setConsentFlowDebugUserGeography(final String userGeographyString)
    {
        val userGeography = ConsentFlowUserGeography.valueOf( userGeographyString );
        sdk.getSettings().getTermsAndPrivacyPolicyFlowSettings().setDebugUserGeography( userGeography );
    }

    public void showCmpForExistingUser()
    {
        if ( !isInitialized() )
        {
            logUninitializedError( "Failed to show CMP flow" );
            return;
        }

        sdk.getCmpService().showCmpForExistingUser( getGameActivity(), this );
    }

    public boolean hasSupportedCmp()
    {
        return sdk.getCmpService().hasSupportedCmp();
    }

    @Override
    public void onCompleted(final AppLovinCmpError error)
    {
        val params = new JSONObject();
        if ( error != null )
        {
            JsonUtils.putInt( params, "code", error.getCode().getValue() );
            JsonUtils.putString( params, "message", error.getMessage() );
            JsonUtils.putInt( params, "cmpCode", error.getCmpCode() );
            JsonUtils.putString( params, "cmpMessage", error.getCmpMessage() );
        }

        sendUnrealEvent( "OnCmpCompletedEvent", params );
    }
    // endregion

    // region General
    public boolean isTablet()
    {
        return AppLovinSdkUtils.isTablet( getGameActivity() );
    }

    public void showMediationDebugger()
    {
        if ( !isInitialized() )
        {
            logUninitializedError( "Failed to show mediation debugger" );
            return;
        }

        sdk.showMediationDebugger();
    }

    public void setUserId(String userId)
    {
        sdk.getSettings().setUserIdentifier( userId );
    }

    public void setMuted(final boolean muted)
    {
        sdk.getSettings().setMuted( muted );
    }

    public boolean isMuted()
    {
        return sdk.getSettings().isMuted();
    }

    public void setVerboseLoggingEnabled(final boolean enabled)
    {
        sdk.getSettings().setVerboseLogging( enabled );
    }

    public boolean isVerboseLoggingEnabled()
    {
        return sdk.getSettings().isVerboseLoggingEnabled();
    }

    public void setCreativeDebuggerEnabled(final boolean enabled)
    {
        sdk.getSettings().setCreativeDebuggerEnabled( enabled );
    }

    public void setTestDeviceAdvertisingIds(final String[] advertisingIds)
    {
        if ( isInitialized() )
        {
            e( "Failed to set test device advertising IDs. Please set your test device advertising IDs before initializing the plugin." );
            return;
        }

        testDeviceAdvertisingIdsToSet = Arrays.asList( advertisingIds );
    }
    // endregion

    // region Event Tracking
    public void trackEvent(final String event, final String parameters)
    {
        if ( !isInitialized() )
        {
            logUninitializedError( "Failed to track event (" + event + ")" );
            return;
        }

        val deserialized = deserialize( parameters );
        sdk.getEventService().trackEvent( event, deserialized );
    }
    // endregion

    // region Banners
    public void createBanner(final String adUnitId, final String bannerPosition)
    {
        createAdView( adUnitId, getDeviceSpecificBannerAdViewAdFormat(), bannerPosition );
    }

    public void setBannerBackgroundColor(final String adUnitId, final String hexColorCode)
    {
        setAdViewBackgroundColor( adUnitId, getDeviceSpecificBannerAdViewAdFormat(), hexColorCode );
    }

    public void setBannerPlacement(final String adUnitId, final String placement)
    {
        setAdViewPlacement( adUnitId, getDeviceSpecificBannerAdViewAdFormat(), placement );
    }

    public void setBannerExtraParameter(final String adUnitId, final String key, final String value)
    {
        setAdViewExtraParameters( adUnitId, getDeviceSpecificBannerAdViewAdFormat(), key, value );
    }

    public void updateBannerPosition(final String adUnitId, final String bannerPosition)
    {
        updateAdViewPosition( adUnitId, bannerPosition, getDeviceSpecificBannerAdViewAdFormat() );
    }

    public void showBanner(final String adUnitId)
    {
        showAdView( adUnitId, getDeviceSpecificBannerAdViewAdFormat() );
    }

    public void hideBanner(final String adUnitId)
    {
        hideAdView( adUnitId, getDeviceSpecificBannerAdViewAdFormat() );
    }

    public void destroyBanner(final String adUnitId)
    {
        destroyAdView( adUnitId, getDeviceSpecificBannerAdViewAdFormat() );
    }
    // endregion

    // region MRECS
    public void createMRec(final String adUnitId, final String mrecPosition)
    {
        createAdView( adUnitId, MaxAdFormat.MREC, mrecPosition );
    }

    public void setMRecPlacement(final String adUnitId, final String placement)
    {
        setAdViewPlacement( adUnitId, MaxAdFormat.MREC, placement );
    }

    public void setMRecExtraParameter(final String adUnitId, final String key, final String value)
    {
        setAdViewExtraParameters( adUnitId, MaxAdFormat.MREC, key, value );
    }

    public void updateMRecPosition(final String adUnitId, final String mrecPosition)
    {
        updateAdViewPosition( adUnitId, mrecPosition, MaxAdFormat.MREC );
    }

    public void showMRec(final String adUnitId)
    {
        showAdView( adUnitId, MaxAdFormat.MREC );
    }

    public void hideMRec(final String adUnitId)
    {
        hideAdView( adUnitId, MaxAdFormat.MREC );
    }

    public void destroyMRec(final String adUnitId)
    {
        destroyAdView( adUnitId, MaxAdFormat.MREC );
    }
    // endregion

    // region Interstitials
    public void loadInterstitial(final String adUnitId)
    {
        val interstitial = retrieveInterstitial( adUnitId );
        interstitial.loadAd();
    }

    public boolean isInterstitialReady(String adUnitId)
    {
        val interstitial = retrieveInterstitial( adUnitId );
        return interstitial.isReady();
    }

    public void showInterstitial(final String adUnitId, final String placement)
    {
        val interstitial = retrieveInterstitial( adUnitId );
        interstitial.showAd( placement, getGameActivity() );
    }

    public void setInterstitialExtraParameter(final String adUnitId, final String key, final String value)
    {
        val interstitial = retrieveInterstitial( adUnitId );
        interstitial.setExtraParameter( key, value );
    }
    // endregion

    // region Rewarded
    public void loadRewardedAd(final String adUnitId)
    {
        val rewardedAd = retrieveRewardedAd( adUnitId );
        rewardedAd.loadAd();
    }

    public boolean isRewardedAdReady(final String adUnitId)
    {
        val rewardedAd = retrieveRewardedAd( adUnitId );
        return rewardedAd.isReady();
    }

    public void showRewardedAd(final String adUnitId, final String placement)
    {
        val rewardedAd = retrieveRewardedAd( adUnitId );
        rewardedAd.showAd( placement, getGameActivity() );
    }

    public void setRewardedAdExtraParameter(final String adUnitId, final String key, final String value)
    {
        val rewardedAd = retrieveRewardedAd( adUnitId );
        rewardedAd.setExtraParameter( key, value );
    }
    // endregion

    // region Ad Callbacks
    @Override
    public void onAdLoaded(@NonNull MaxAd ad)
    {
        val adFormat = ad.getFormat();
        if ( isInvalidAdFormat( adFormat ) )
        {
            logInvalidAdFormat( adFormat );
            return;
        }

        if ( adFormat.isAdViewAd() )
        {
            val adViewPosition = adViewPositions.get( ad.getAdUnitId() );
            if ( !TextUtils.isEmpty( adViewPosition ) )
            {
                // Only position ad if not native UI component
                positionAdView( ad );
            }

            // Do not auto-refresh by default if the ad view is not showing yet (e.g. first load during app launch and publisher does not automatically show banner upon load success)
            // We will resume auto-refresh in {@link #showBanner(String)}.
            val adView = retrieveAdView( ad.getAdUnitId(), adFormat );
            if ( adView != null && adView.getVisibility() != View.VISIBLE )
            {
                adView.stopAutoRefresh();
            }
        }

        val name = getEventName( adFormat, "Loaded" );
        sendUnrealEvent( name, getAdInfo( ad ) );
    }

    @Override
    public void onAdLoadFailed(@NonNull final String adUnitId, @NonNull final MaxError error)
    {
        if ( TextUtils.isEmpty( adUnitId ) )
        {
            logStackTrace( new IllegalArgumentException( "Ad unit ID cannot be null" ) );
            return;
        }

        // Determine ad format
        final MaxAdFormat adFormat;
        if ( adViews.containsKey( adUnitId ) )
        {
            adFormat = adViewAdFormats.get( adUnitId );
        }
        else if ( interstitials.containsKey( adUnitId ) )
        {
            adFormat = MaxAdFormat.INTERSTITIAL;
        }
        else if ( rewardedAds.containsKey( adUnitId ) )
        {
            adFormat = MaxAdFormat.REWARDED;
        }
        else
        {
            logStackTrace( new IllegalStateException( "Invalid ad unit ID: " + adUnitId ) );
            return;
        }

        val name = getEventName( adFormat, "LoadFailed" );
        val params = getErrorInfo( error );
        JsonUtils.putString( params, "adUnitIdentifier", adUnitId );

        sendUnrealEvent( name, params );
    }

    @Override
    public void onAdClicked(@NonNull final MaxAd ad)
    {
        val adFormat = ad.getFormat();
        if ( isInvalidAdFormat( adFormat ) )
        {
            logInvalidAdFormat( adFormat );
            return;
        }

        val name = getEventName( adFormat, "Clicked" );
        sendUnrealEvent( name, getAdInfo( ad ) );
    }

    @Override
    public void onAdDisplayed(@NonNull final MaxAd ad)
    {
        // BMLs do not support [DISPLAY] events
        val adFormat = ad.getFormat();
        if ( adFormat.isAdViewAd() )
        {
            logInvalidAdFormat( adFormat );
            return;
        }

        val name = getEventName( adFormat, "Displayed" );
        sendUnrealEvent( name, getAdInfo( ad ) );
    }

    @Override
    public void onAdDisplayFailed(@NonNull final MaxAd ad, @NonNull final MaxError error)
    {
        // BMLs do not support [DISPLAY] events
        val adFormat = ad.getFormat();
        if ( adFormat.isAdViewAd() )
        {
            logInvalidAdFormat( adFormat );
            return;
        }

        val params = getAdInfo( ad );
        JsonUtils.putAll( params, getErrorInfo( error ) );

        val name = getEventName( adFormat, "DisplayFailed" );
        sendUnrealEvent( name, params );
    }

    @Override
    public void onAdHidden(@NonNull final MaxAd ad)
    {
        // BMLs do not support [HIDDEN] events
        val adFormat = ad.getFormat();
        if ( adFormat.isAdViewAd() )
        {
            logInvalidAdFormat( adFormat );
            return;
        }

        val name = getEventName( adFormat, "Hidden" );
        sendUnrealEvent( name, getAdInfo( ad ) );
    }

    @Override
    public void onAdExpanded(@NonNull final MaxAd ad)
    {
        val adFormat = ad.getFormat();
        if ( !adFormat.isAdViewAd() )
        {
            logInvalidAdFormat( adFormat );
            return;
        }

        val name = getEventName( adFormat, "Expanded" );
        sendUnrealEvent( name, getAdInfo( ad ) );
    }

    @Override
    public void onAdCollapsed(@NonNull final MaxAd ad)
    {
        val adFormat = ad.getFormat();
        if ( !adFormat.isAdViewAd() )
        {
            logInvalidAdFormat( adFormat );
            return;
        }

        val name = getEventName( adFormat, "Collapsed" );
        sendUnrealEvent( name, getAdInfo( ad ) );
    }

    @Override
    public void onUserRewarded(@NonNull final MaxAd ad, @NonNull final MaxReward reward)
    {
        val adFormat = ad.getFormat();
        if ( adFormat != MaxAdFormat.REWARDED )
        {
            logInvalidAdFormat( adFormat );
            return;
        }

        val params = getAdInfo( ad );
        JsonUtils.putString( params, "label", reward.getLabel() );
        JsonUtils.putInt( params, "amount", reward.getAmount() );

        val name = getEventName( adFormat, "ReceivedReward" );
        sendUnrealEvent( name, params );
    }

    @Override
    public void onAdRevenuePaid(@NonNull final MaxAd ad)
    {
        val adFormat = ad.getFormat();
        if ( isInvalidAdFormat( adFormat ) )
        {
            logInvalidAdFormat( adFormat );
            return;
        }

        val name = getEventName( adFormat, "AdRevenuePaid" );
        sendUnrealEvent( name, getAdInfo( ad ) );
    }

    // region Internal Methods
    private void createAdView(final String adUnitId, final MaxAdFormat adFormat, final String adViewPosition)
    {
        // Run on main thread to ensure there are no concurrency issues with other ad view methods
        getGameActivity().runOnUiThread( () -> {
            d( "Creating " + adFormat.getLabel() + " with ad unit id \"" + adUnitId + "\" and position: \"" + adViewPosition + "\"" );

            // Retrieve ad view from the map
            val adView = retrieveAdView( adUnitId, adFormat, adViewPosition );
            if ( adView == null )
            {
                e( adFormat.getLabel() + " does not exist" );
                return;
            }

            adView.setVisibility( View.GONE );

            if ( adView.getParent() == null )
            {
                val currentActivity = getGameActivity();
                val relativeLayout = new RelativeLayout( currentActivity );
                currentActivity.addContentView( relativeLayout, new LinearLayout.LayoutParams( LinearLayout.LayoutParams.MATCH_PARENT,
                                                                                               LinearLayout.LayoutParams.MATCH_PARENT ) );
                relativeLayout.addView( adView );

                // Position ad view immediately so if publisher sets color before ad loads, it will not be the size of the screen
                adViewAdFormats.put( adUnitId, adFormat );
                positionAdView( adUnitId, adFormat );
            }

            adView.loadAd();

            // The publisher may have requested to show the banner before it was created. Now that the banner is created, show it.
            if ( adUnitIdsToShowAfterCreate.contains( adUnitId ) )
            {
                showAdView( adUnitId, adFormat );
                adUnitIdsToShowAfterCreate.remove( adUnitId );
            }
        } );
    }

    private void setAdViewPlacement(final String adUnitId, final MaxAdFormat adFormat, final String placement)
    {
        getGameActivity().runOnUiThread( () -> {
            d( "Setting placement \"" + placement + "\" for " + adFormat.getLabel() + " with ad unit id \"" + adUnitId + "\"" );

            val adView = retrieveAdView( adUnitId, adFormat );
            if ( adView == null )
            {
                e( adFormat.getLabel() + " does not exist" );
                return;
            }

            adView.setPlacement( placement );
        } );
    }

    private void updateAdViewPosition(final String adUnitId, final String adViewPosition, final MaxAdFormat adFormat)
    {
        getGameActivity().runOnUiThread( () -> {
            d( "Updating " + adFormat.getLabel() + " position to \"" + adViewPosition + "\" for ad unit id \"" + adUnitId + "\"" );

            // Retrieve ad view from the map
            val adView = retrieveAdView( adUnitId, adFormat );
            if ( adView == null )
            {
                e( adFormat.getLabel() + " does not exist" );
                return;
            }

            // Check if the previous position is same as the new position. If so, no need to update the position again.
            val previousPosition = adViewPositions.get( adUnitId );
            if ( adViewPosition == null || adViewPosition.equals( previousPosition ) ) return;

            adViewPositions.put( adUnitId, adViewPosition );
            positionAdView( adUnitId, adFormat );
        } );
    }

    private void showAdView(final String adUnitId, final MaxAdFormat adFormat)
    {
        getGameActivity().runOnUiThread( () -> {
            d( "Showing " + adFormat.getLabel() + " with ad unit id \"" + adUnitId + "\"" );

            val adView = retrieveAdView( adUnitId, adFormat );
            if ( adView == null )
            {
                e( adFormat.getLabel() + " does not exist for ad unit id " + adUnitId );

                // The adView has not yet been created. Store the ad unit ID, so that it can be displayed once the banner has been created.
                adUnitIdsToShowAfterCreate.add( adUnitId );
                return;
            }

            adView.setVisibility( View.VISIBLE );
            adView.startAutoRefresh();
        } );
    }

    private void hideAdView(final String adUnitId, final MaxAdFormat adFormat)
    {
        getGameActivity().runOnUiThread( () -> {
            d( "Hiding " + adFormat.getLabel() + " with ad unit id \"" + adUnitId + "\"" );
            adUnitIdsToShowAfterCreate.remove( adUnitId );

            val adView = retrieveAdView( adUnitId, adFormat );
            if ( adView == null )
            {
                e( adFormat.getLabel() + " does not exist" );
                return;
            }

            adView.setVisibility( View.GONE );
            adView.stopAutoRefresh();
        } );
    }

    private void destroyAdView(final String adUnitId, final MaxAdFormat adFormat)
    {
        getGameActivity().runOnUiThread( () -> {
            d( "Destroying " + adFormat.getLabel() + " with ad unit id \"" + adUnitId + "\"" );

            val adView = retrieveAdView( adUnitId, adFormat );
            if ( adView == null )
            {
                e( adFormat.getLabel() + " does not exist" );
                return;
            }

            val parent = adView.getParent();
            if ( parent instanceof ViewGroup )
            {
                ( (ViewGroup) parent ).removeView( adView );
            }

            adView.setListener( null );
            adView.destroy();

            adViews.remove( adUnitId );
            adViewAdFormats.remove( adUnitId );
            adViewPositions.remove( adUnitId );
            verticalAdViewFormats.remove( adUnitId );
        } );
    }

    private void setAdViewBackgroundColor(final String adUnitId, final MaxAdFormat adFormat, final String hexColorCode)
    {
        getGameActivity().runOnUiThread( () -> {
            d( "Setting " + adFormat.getLabel() + " with ad unit id \"" + adUnitId + "\" to color: " + hexColorCode );

            val adView = retrieveAdView( adUnitId, adFormat );
            if ( adView == null )
            {
                e( adFormat.getLabel() + " does not exist" );
                return;
            }

            adView.setBackgroundColor( Color.parseColor( hexColorCode ) );
        } );
    }

    private void setAdViewExtraParameters(final String adUnitId, final MaxAdFormat adFormat, final String key, final String value)
    {
        getGameActivity().runOnUiThread( () -> {
            d( "Setting " + adFormat.getLabel() + " extra with key: \"" + key + "\" value: " + value );

            // Retrieve ad view from the map
            val adView = retrieveAdView( adUnitId, adFormat );
            if ( adView == null )
            {
                e( adFormat.getLabel() + " does not exist" );
                return;
            }

            adView.setExtraParameter( key, value );

            // Handle local changes as needed
            if ( "force_banner".equalsIgnoreCase( key ) && MaxAdFormat.MREC != adFormat )
            {
                val shouldForceBanner = Boolean.parseBoolean( value );
                val forcedAdFormat = shouldForceBanner ? MaxAdFormat.BANNER : getDeviceSpecificBannerAdViewAdFormat();

                adViewAdFormats.put( adUnitId, forcedAdFormat );
                positionAdView( adUnitId, forcedAdFormat );
            }
        } );
    }

    private MaxInterstitialAd retrieveInterstitial(String adUnitId)
    {
        var result = interstitials.get( adUnitId );
        if ( result == null )
        {
            result = new MaxInterstitialAd( adUnitId, sdk, getGameActivity() );
            result.setListener( this );

            interstitials.put( adUnitId, result );
        }

        return result;
    }

    private MaxRewardedAd retrieveRewardedAd(String adUnitId)
    {
        var result = rewardedAds.get( adUnitId );
        if ( result == null )
        {
            result = MaxRewardedAd.getInstance( adUnitId, sdk, getGameActivity() );
            result.setListener( this );

            rewardedAds.put( adUnitId, result );
        }

        return result;
    }

    private MaxAdView retrieveAdView(String adUnitId, MaxAdFormat adFormat)
    {
        return retrieveAdView( adUnitId, adFormat, null );
    }

    public MaxAdView retrieveAdView(String adUnitId, MaxAdFormat adFormat, String adViewPosition)
    {
        var result = adViews.get( adUnitId );
        if ( result == null && adViewPosition != null )
        {
            // Must explicitly cast the GameActivity to Context to avoid a crash from NoSuchMethodError
            result = new MaxAdView( adUnitId, adFormat, sdk, (Context) getGameActivity() );
            result.setListener( this );

            adViews.put( adUnitId, result );
            adViewPositions.put( adUnitId, adViewPosition );
        }

        return result;
    }

    private void positionAdView(MaxAd ad)
    {
        positionAdView( ad.getAdUnitId(), ad.getFormat() );
    }

    private void positionAdView(String adUnitId, MaxAdFormat adFormat)
    {
        val adView = retrieveAdView( adUnitId, adFormat );
        if ( adView == null )
        {
            e( adFormat.getLabel() + " does not exist" );
            return;
        }

        val relativeLayout = (RelativeLayout) adView.getParent();
        if ( relativeLayout == null )
        {
            e( adFormat.getLabel() + "'s parent does not exist" );
            return;
        }

        // Size the ad
        val adViewPosition = adViewPositions.get( adUnitId );
        val adViewSize = adFormat.getSize();
        val width = AppLovinSdkUtils.dpToPx( getGameActivity(), adViewSize.getWidth() );
        val height = AppLovinSdkUtils.dpToPx( getGameActivity(), adViewSize.getHeight() );

        val params = (RelativeLayout.LayoutParams) adView.getLayoutParams();
        params.height = height;
        adView.setLayoutParams( params );

        // Parse gravity
        var gravity = 0;

        // Reset rotation, translation and margins so that the banner can be positioned again
        adView.setRotation( 0 );
        adView.setTranslationX( 0 );
        params.setMargins( 0, 0, 0, 0 );
        verticalAdViewFormats.remove( adUnitId );

        if ( "centered".equalsIgnoreCase( adViewPosition ) )
        {
            gravity = Gravity.CENTER_VERTICAL | Gravity.CENTER_HORIZONTAL;
        }
        else
        {
            // Figure out vertical params
            if ( adViewPosition == null )
            {
                e( "Error positioning ad view due to null position" );
                return;
            }
            else if ( adViewPosition.contains( "top" ) )
            {
                gravity = Gravity.TOP;
            }
            else if ( adViewPosition.contains( "bottom" ) )
            {
                gravity = Gravity.BOTTOM;
            }

            // Figure out horizontal params
            if ( adViewPosition.contains( "center" ) )
            {
                gravity |= Gravity.CENTER_HORIZONTAL;
                params.width = ( MaxAdFormat.MREC == adFormat ) ? width : RelativeLayout.LayoutParams.MATCH_PARENT; // Stretch width if banner

                // Check if the publisher wants the ad view to be vertical and update the position accordingly ('CenterLeft' or 'CenterRight').
                val containsLeft = adViewPosition.contains( "left" );
                val containsRight = adViewPosition.contains( "right" );
                if ( containsLeft || containsRight )
                {
                    // First, center the ad view in the view.
                    gravity |= Gravity.CENTER_VERTICAL;

                    // For banners, set the width to the height of the screen to span the ad across the screen after it is rotated.
                    // Android by default clips a view bounds if it goes over the size of the screen. We can overcome it by setting negative margins to match our required size.
                    if ( MaxAdFormat.MREC == adFormat )
                    {
                        gravity |= adViewPosition.contains( "left" ) ? Gravity.LEFT : Gravity.RIGHT;
                    }
                    else
                    {
                        /* Align the center of the view such that when rotated it snaps into place.
                         *
                         *                  +---+---+-------+
                         *                  |   |           |
                         *                  |   |           |
                         *                  |   |           |
                         *                  |   |           |
                         *                  |   |           |
                         *                  |   |           |
                         *    +-------------+---+-----------+--+
                         *    |             | + |   +       |  |
                         *    +-------------+---+-----------+--+
                         *                  |   |           |
                         *                  | ^ |   ^       |
                         *                  | +-----+       |
                         *                  Translation     |
                         *                  |   |           |
                         *                  |   |           |
                         *                  +---+-----------+
                         */
                        val windowRect = new Rect();
                        relativeLayout.getWindowVisibleDisplayFrame( windowRect );

                        val windowWidth = windowRect.width();
                        val windowHeight = windowRect.height();
                        val longSide = Math.max( windowWidth, windowHeight );
                        val shortSide = Math.min( windowWidth, windowHeight );
                        val margin = ( longSide - shortSide ) / 2;
                        params.setMargins( -margin, 0, -margin, 0 );

                        // The view is now at the center of the screen and so is it's pivot point. Move its center such that when rotated, it snaps into the vertical position we need.
                        val translationRaw = ( windowWidth / 2 ) - ( height / 2 );
                        val translationX = containsLeft ? -translationRaw : translationRaw;
                        adView.setTranslationX( translationX );

                        // We have the view's center in the correct position. Now rotate it to snap into place.
                        adView.setRotation( 270 );

                        // Store the ad view with format, so that it can be updated when the orientation changes.
                        verticalAdViewFormats.put( adUnitId, adFormat );
                    }
                }
            }
            else
            {
                params.width = width;

                if ( adViewPosition.contains( "left" ) )
                {
                    gravity |= Gravity.LEFT;
                }
                else if ( adViewPosition.contains( "right" ) )
                {
                    gravity |= Gravity.RIGHT;
                }
            }
        }

        relativeLayout.setGravity( gravity );
    }
    // endregion

    // region Utility Methods
    private MaxAdFormat getDeviceSpecificBannerAdViewAdFormat()
    {
        return getDeviceSpecificBannerAdViewAdFormat( getGameActivity() );
    }

    public static MaxAdFormat getDeviceSpecificBannerAdViewAdFormat(final Context context)
    {
        return AppLovinSdkUtils.isTablet( context ) ? MaxAdFormat.LEADER : MaxAdFormat.BANNER;
    }

    private boolean isInvalidAdFormat(final MaxAdFormat adFormat)
    {
        return adFormat == null || !( adFormat.isAdViewAd() || MaxAdFormat.INTERSTITIAL == adFormat || MaxAdFormat.REWARDED == adFormat );
    }

    private JSONObject getAdInfo(final MaxAd ad)
    {
        val adInfo = new JSONObject();

        JsonUtils.putString( adInfo, "adUnitIdentifier", ad.getAdUnitId() );
        JsonUtils.putString( adInfo, "creativeIdentifier", StringUtils.emptyIfNull( ad.getCreativeId() ) );
        JsonUtils.putString( adInfo, "networkName", ad.getNetworkName() );
        JsonUtils.putString( adInfo, "placement", StringUtils.emptyIfNull( ad.getPlacement() ) );
        JsonUtils.putDouble( adInfo, "revenue", ad.getRevenue() );

        return adInfo;
    }

    private JSONObject getErrorInfo(final MaxError error)
    {
        val errorInfo = new JSONObject();

        JsonUtils.putInt( errorInfo, "code", error.getCode() );
        JsonUtils.putString( errorInfo, "message", error.getMessage() );
        JsonUtils.putString( errorInfo, "waterfall", error.getWaterfall() != null ? error.getWaterfall().toString() : "" );

        return errorInfo;
    }

    private String getEventName(final MaxAdFormat adFormat, final String event)
    {
        if ( adFormat != null )
        {
            if ( adFormat.isBannerOrLeaderAd() )
            {
                return "OnBannerAd" + event + "Event";
            }
            else if ( MaxAdFormat.MREC == adFormat )
            {
                return "OnMRecAd" + event + "Event";
            }
            else if ( MaxAdFormat.INTERSTITIAL == adFormat )
            {
                return "OnInterstitialAd" + event + "Event";
            }
            else if ( MaxAdFormat.REWARDED == adFormat )
            {
                return "OnRewardedAd" + event + "Event";
            }
        }

        throw new IllegalArgumentException( "Invalid ad format" );
    }

    private static Map<String, String> deserialize(final String serialized)
    {
        if ( !TextUtils.isEmpty( serialized ) )
        {
            try
            {
                return JsonUtils.toStringMap( JsonUtils.jsonObjectFromJsonString( serialized, new JSONObject() ) );
            }
            catch ( Throwable th )
            {
                e( "Failed to deserialize: (" + serialized + ") with exception: " + th );
            }
        }

        return Collections.emptyMap();
    }
    // endregion

    //region Logging
    public static void d(final String message)
    {
        Log.d( SDK_TAG, "[" + TAG + "] " + message );
    }

    public static void e(final String message)
    {
        Log.e( SDK_TAG, "[" + TAG + "] " + message );
    }

    private void logInvalidAdFormat(MaxAdFormat adFormat)
    {
        logStackTrace( new IllegalStateException( "Invalid ad format: " + adFormat ) );
    }

    private void logStackTrace(Exception e)
    {
        e( Log.getStackTraceString( e ) );
    }

    private void logUninitializedError(String message)
    {
        e( message + " - " + "Please ensure the AppLovin MAX Unreal Plugin has been initialized by calling UAppLovinMAX::Initialize()" );
    }
    //endregion

    // region Unreal Bridge

    // NOTE: Unreal deserializes to the relevant USTRUCT based on the JSON keys, so the keys must match with the corresponding UPROPERTY
    private void sendUnrealEvent(final String name, final JSONObject params)
    {
        if ( eventListener == null ) return;
        eventListener.onReceivedEvent( name, params.toString() );
    }
    // endregion
}
