package com.applovin.unreal;

import android.app.Activity;
import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.graphics.Color;
import android.graphics.Rect;
import android.os.Bundle;
import android.text.TextUtils;
import android.util.Log;
import android.view.Gravity;
import android.view.OrientationEventListener;
import android.view.View;
import android.view.ViewGroup;
import android.view.ViewParent;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;

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
import com.applovin.sdk.AppLovinMediationProvider;
import com.applovin.sdk.AppLovinPrivacySettings;
import com.applovin.sdk.AppLovinSdk;
import com.applovin.sdk.AppLovinSdkConfiguration;
import com.applovin.sdk.AppLovinSdkSettings;
import com.applovin.sdk.AppLovinSdkUtils;

import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * Created by Ritam Sarmah on May 5 2021
 */
public class MaxUnrealPlugin
        implements MaxAdListener, MaxAdViewAdListener, MaxRewardedAdListener, MaxAdRevenueListener
{
    private static final String TAG     = "MaxUnrealPlugin";
    private static final String SDK_TAG = "AppLovinSdk";

    private static final String SERIALIZED_KEY_VALUE_SEPARATOR      = String.valueOf( (char) 28 );
    private static final String SERIALIZED_KEY_VALUE_PAIR_SEPARATOR = String.valueOf( (char) 29 );

    // Parent Fields
    private AppLovinSdk              sdk;
    private boolean                  isPluginInitialized = false;
    private boolean                  isSdkInitialized    = false;
    private AppLovinSdkConfiguration sdkConfiguration;

    // Store these values if pub attempts to set it before initializing
    private String       userIdToSet;
    private List<String> testDeviceAdvertisingIdsToSet;
    private Boolean      verboseLoggingToSet;
    private Boolean      creativeDebuggerEnabledToSet;

    // Fullscreen Ad Fields
    private final Map<String, MaxInterstitialAd> mInterstitials = new HashMap<>( 2 );
    private final Map<String, MaxRewardedAd>     mRewardedAds   = new HashMap<>( 2 );

    // Banner Fields
    private final Map<String, MaxAdView>   mAdViews                    = new HashMap<>( 2 );
    private final Map<String, MaxAdFormat> mAdViewAdFormats            = new HashMap<>( 2 );
    private final Map<String, String>      mAdViewPositions            = new HashMap<>( 2 );
    private final Map<String, MaxAdFormat> mVerticalAdViewFormats      = new HashMap<>( 2 );
    private final List<String>             mAdUnitIdsToShowAfterCreate = new ArrayList<>( 2 );

    private final WeakReference<Activity> gameActivity;
    private       EventListener           eventListener;

    private Activity getGameActivity() { return gameActivity.get(); }

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
        gameActivity = new WeakReference<>( activity );
    }

    public void initialize(final String pluginVersion, final String sdkKey, final EventListener listener)
    {
        // Check if Activity is available
        Activity currentActivity = getGameActivity();
        if ( currentActivity == null ) throw new IllegalStateException( "No Activity found" );

        final Context context = currentActivity.getApplicationContext();

        // Guard against running init logic multiple times
        if ( isPluginInitialized )
        {
            sendUnrealEvent( "OnSdkInitializedEvent", getInitializationMessage( context ) );
            return;
        }

        isPluginInitialized = true;

        d( "Initializing AppLovin MAX Unreal v" + pluginVersion + "..." );

        // Set listener
        eventListener = listener;

        // If SDK key passed in is empty, check Android Manifest
        String sdkKeyToUse = sdkKey;
        if ( TextUtils.isEmpty( sdkKey ) )
        {
            try
            {
                PackageManager packageManager = context.getPackageManager();
                String packageName = context.getPackageName();
                ApplicationInfo applicationInfo = packageManager.getApplicationInfo( packageName, PackageManager.GET_META_DATA );
                Bundle metaData = applicationInfo.metaData;

                sdkKeyToUse = metaData.getString( "applovin.sdk.key", "" );
            }
            catch ( Throwable th )
            {
                e( "Unable to retrieve SDK key from Android Manifest: " + th );
            }

            if ( TextUtils.isEmpty( sdkKeyToUse ) )
            {
                throw new IllegalStateException( "Unable to initialize AppLovin SDK - no SDK key provided and not found in Android Manifest!" );
            }
        }

        // Initialize SDK
        sdk = AppLovinSdk.getInstance( sdkKeyToUse, new AppLovinSdkSettings( context ), currentActivity );
        sdk.setPluginVersion( "Unreal-" + pluginVersion );
        sdk.setMediationProvider( AppLovinMediationProvider.MAX );

        // Set user id if needed
        if ( !TextUtils.isEmpty( userIdToSet ) )
        {
            sdk.setUserIdentifier( userIdToSet );
            userIdToSet = null;
        }

        // Set test device ids if needed
        if ( testDeviceAdvertisingIdsToSet != null )
        {
            sdk.getSettings().setTestDeviceAdvertisingIds( testDeviceAdvertisingIdsToSet );
            testDeviceAdvertisingIdsToSet = null;
        }

        // Set verbose logging state if needed
        if ( verboseLoggingToSet != null )
        {
            sdk.getSettings().setVerboseLogging( verboseLoggingToSet );
            verboseLoggingToSet = null;
        }

        // Set creative debugger enabled if needed
        if ( creativeDebuggerEnabledToSet != null )
        {
            sdk.getSettings().setCreativeDebuggerEnabled( creativeDebuggerEnabledToSet );
            creativeDebuggerEnabledToSet = null;
        }

        sdk.initializeSdk( new AppLovinSdk.SdkInitializationListener()
        {
            @Override
            public void onSdkInitialized(final AppLovinSdkConfiguration configuration)
            {
                d( "SDK initialized" );

                sdkConfiguration = configuration;
                isSdkInitialized = true;

                // Enable orientation change listener, so that the position can be updated for vertical banners.
                new OrientationEventListener( context )
                {
                    @Override
                    public void onOrientationChanged(final int orientation)
                    {
                        for ( final Map.Entry<String, MaxAdFormat> adUnitFormats : mVerticalAdViewFormats.entrySet() )
                        {
                            positionAdView( adUnitFormats.getKey(), adUnitFormats.getValue() );
                        }
                    }
                }.enable();

                sendUnrealEvent( "OnSdkInitializedEvent", getInitializationMessage( context ) );
            }
        } );
    }

    private Map<String, String> getInitializationMessage(final Context context)
    {
        Map<String, String> message = new HashMap<>( 6 );

        if ( sdkConfiguration != null )
        {
            message.put( "consentDialogState", String.valueOf( sdkConfiguration.getConsentDialogState().ordinal() ) );
            message.put( "countryCode", sdkConfiguration.getCountryCode() );
        }
        else
        {
            message.put( "consentDialogState", String.valueOf( AppLovinSdkConfiguration.ConsentDialogState.UNKNOWN.ordinal() ) );
        }

        // Use "true" string to represent boolean value of true
        message.put( "hasUserConsent", String.valueOf( AppLovinPrivacySettings.hasUserConsent( context ) ) );
        message.put( "isAgeRestrictedUser", String.valueOf( AppLovinPrivacySettings.isAgeRestrictedUser( context ) ) );
        message.put( "isDoNotSell", String.valueOf( AppLovinPrivacySettings.isDoNotSell( context ) ) );
        message.put( "isTablet", String.valueOf( AppLovinSdkUtils.isTablet( context ) ) );

        return message;
    }

    public boolean isInitialized()
    {
        return isPluginInitialized && isSdkInitialized;
    }

    public void showMediationDebugger()
    {
        if ( sdk == null )
        {
            Log.e( "[" + TAG + "]", "Failed to show mediation debugger - please ensure the AppLovin MAX Unreal Plugin has been initialized by calling 'UAppLovinMAX::Initialize()'!" );
            return;
        }

        sdk.showMediationDebugger();
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

    public void setIsAgeRestrictedUser(final boolean isAgeRestrictedUser)
    {
        AppLovinPrivacySettings.setIsAgeRestrictedUser( isAgeRestrictedUser, getGameActivity() );
    }

    public boolean isAgeRestrictedUser()
    {
        return AppLovinPrivacySettings.isAgeRestrictedUser( getGameActivity() );
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

    // region General
    public boolean isTablet()
    {
        return AppLovinSdkUtils.isTablet( getGameActivity() );
    }

    public void setUserId(String userId)
    {
        if ( sdk != null )
        {
            sdk.setUserIdentifier( userId );
            userIdToSet = null;
        }
        else
        {
            userIdToSet = userId;
        }
    }

    public void setMuted(final boolean muted)
    {
        if ( !isPluginInitialized ) return;

        sdk.getSettings().setMuted( muted );
    }

    public boolean isMuted()
    {
        if ( !isPluginInitialized ) return false;

        return sdk.getSettings().isMuted();
    }

    public void setVerboseLoggingEnabled(final boolean enabled)
    {
        if ( sdk != null )
        {
            sdk.getSettings().setVerboseLogging( enabled );
            verboseLoggingToSet = null;
        }
        else
        {
            verboseLoggingToSet = enabled;
        }
    }

    public boolean isVerboseLoggingEnabled()
    {
        if ( sdk != null )
        {
            return sdk.getSettings().isVerboseLoggingEnabled();
        }
        else if ( verboseLoggingToSet != null )
        {
            return verboseLoggingToSet;
        }

        return false;
    }

    public void setCreativeDebuggerEnabled(final boolean enabled)
    {
        if ( sdk != null )
        {
            sdk.getSettings().setCreativeDebuggerEnabled( enabled );
            creativeDebuggerEnabledToSet = null;
        }
        else
        {
            creativeDebuggerEnabledToSet = enabled;
        }
    }

    public void setTestDeviceAdvertisingIds(final String[] advertisingIds)
    {
        if ( isPluginInitialized )
        {
            sdk.getSettings().setTestDeviceAdvertisingIds( Arrays.asList( advertisingIds ) );
            testDeviceAdvertisingIdsToSet = null;
        }
        else
        {
            testDeviceAdvertisingIdsToSet = Arrays.asList( advertisingIds );
        }
    }
    // endregion

    // region Event Tracking
    public void trackEvent(final String event, final String parameters)
    {
        if ( sdk == null ) return;

        final Map<String, String> deserialized = deserialize( parameters );
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
        MaxInterstitialAd interstitial = retrieveInterstitial( adUnitId );
        interstitial.loadAd();
    }

    public boolean isInterstitialReady(String adUnitId)
    {
        MaxInterstitialAd interstitial = retrieveInterstitial( adUnitId );
        return interstitial.isReady();
    }

    public void showInterstitial(final String adUnitId, final String placement)
    {
        MaxInterstitialAd interstitial = retrieveInterstitial( adUnitId );
        interstitial.showAd( placement );
    }

    public void setInterstitialExtraParameter(final String adUnitId, final String key, final String value)
    {
        MaxInterstitialAd interstitial = retrieveInterstitial( adUnitId );
        interstitial.setExtraParameter( key, value );
    }
    // endregion

    // region Rewarded
    public void loadRewardedAd(final String adUnitId)
    {
        MaxRewardedAd rewardedAd = retrieveRewardedAd( adUnitId );
        rewardedAd.loadAd();
    }

    public boolean isRewardedAdReady(final String adUnitId)
    {
        MaxRewardedAd rewardedAd = retrieveRewardedAd( adUnitId );
        return rewardedAd.isReady();
    }

    public void showRewardedAd(final String adUnitId, final String placement)
    {
        MaxRewardedAd rewardedAd = retrieveRewardedAd( adUnitId );
        rewardedAd.showAd( placement );
    }

    public void setRewardedAdExtraParameter(final String adUnitId, final String key, final String value)
    {
        MaxRewardedAd rewardedAd = retrieveRewardedAd( adUnitId );
        rewardedAd.setExtraParameter( key, value );
    }
    // endregion

    // region Ad Callbacks
    @Override
    public void onAdLoaded(MaxAd ad)
    {
        String name;
        MaxAdFormat adFormat = ad.getFormat();
        if ( MaxAdFormat.BANNER == adFormat || MaxAdFormat.LEADER == adFormat || MaxAdFormat.MREC == adFormat )
        {
            name = ( MaxAdFormat.MREC == adFormat ) ? "OnMRecAdLoadedEvent" : "OnBannerAdLoadedEvent";

            String adViewPosition = mAdViewPositions.get( ad.getAdUnitId() );
            if ( !TextUtils.isEmpty( adViewPosition ) )
            {
                // Only position ad if not native UI component
                positionAdView( ad );
            }

            // Do not auto-refresh by default if the ad view is not showing yet (e.g. first load during app launch and publisher does not automatically show banner upon load success)
            // We will resume auto-refresh in {@link #showBanner(String)}.
            MaxAdView adView = retrieveAdView( ad.getAdUnitId(), adFormat );
            if ( adView != null && adView.getVisibility() != View.VISIBLE )
            {
                adView.stopAutoRefresh();
            }
        }
        else if ( MaxAdFormat.INTERSTITIAL == adFormat )
        {
            name = "OnInterstitialLoadedEvent";
        }
        else if ( MaxAdFormat.REWARDED == adFormat )
        {
            name = "OnRewardedAdLoadedEvent";
        }
        else
        {
            logInvalidAdFormat( adFormat );
            return;
        }

        sendUnrealEvent( name, getAdInfo( ad ) );
    }

    @Override
    public void onAdLoadFailed(final String adUnitId, final MaxError error)
    {
        if ( TextUtils.isEmpty( adUnitId ) )
        {
            logStackTrace( new IllegalArgumentException( "adUnitId cannot be null" ) );
            return;
        }

        String name;
        if ( mAdViews.containsKey( adUnitId ) )
        {
            name = ( MaxAdFormat.MREC == mAdViewAdFormats.get( adUnitId ) ) ? "OnMRecAdLoadFailedEvent" : "OnBannerAdLoadFailedEvent";
        }
        else if ( mInterstitials.containsKey( adUnitId ) )
        {
            name = "OnInterstitialLoadFailedEvent";
        }
        else if ( mRewardedAds.containsKey( adUnitId ) )
        {
            name = "OnRewardedAdLoadFailedEvent";
        }
        else
        {
            logStackTrace( new IllegalStateException( "invalid adUnitId: " + adUnitId ) );
            return;
        }

        Map<String, String> params = new HashMap<>();
        params.put( "adUnitId", adUnitId );
        params.putAll( getErrorInfo( error ) );
        sendUnrealEvent( name, params );
    }

    @Override
    public void onAdClicked(final MaxAd ad)
    {
        final MaxAdFormat adFormat = ad.getFormat();
        final String name;
        if ( MaxAdFormat.BANNER == adFormat || MaxAdFormat.LEADER == adFormat )
        {
            name = "OnBannerAdClickedEvent";
        }
        else if ( MaxAdFormat.MREC == adFormat )
        {
            name = "OnMRecAdClickedEvent";
        }
        else if ( MaxAdFormat.INTERSTITIAL == adFormat )
        {
            name = "OnInterstitialClickedEvent";
        }
        else if ( MaxAdFormat.REWARDED == adFormat )
        {
            name = "OnRewardedAdClickedEvent";
        }
        else
        {
            logInvalidAdFormat( adFormat );
            return;
        }

        sendUnrealEvent( name, getAdInfo( ad ) );
    }

    @Override
    public void onAdDisplayed(final MaxAd ad)
    {
        // BMLs do not support [DISPLAY] events
        final MaxAdFormat adFormat = ad.getFormat();
        if ( adFormat != MaxAdFormat.INTERSTITIAL && adFormat != MaxAdFormat.REWARDED ) return;

        final String name;
        if ( MaxAdFormat.INTERSTITIAL == adFormat )
        {
            name = "OnInterstitialDisplayedEvent";
        }
        else // REWARDED
        {
            name = "OnRewardedAdDisplayedEvent";
        }

        sendUnrealEvent( name, getAdInfo( ad ) );
    }

    @Override
    public void onAdDisplayFailed(final MaxAd ad, final MaxError error)
    {
        // BMLs do not support [DISPLAY] events
        final MaxAdFormat adFormat = ad.getFormat();
        if ( adFormat != MaxAdFormat.INTERSTITIAL && adFormat != MaxAdFormat.REWARDED ) return;

        final String name;
        if ( MaxAdFormat.INTERSTITIAL == adFormat )
        {
            name = "OnInterstitialAdFailedToDisplayEvent";
        }
        else // REWARDED
        {
            name = "OnRewardedAdFailedToDisplayEvent";
        }

        Map<String, String> params = new HashMap<>();
        params.putAll( getAdInfo( ad ) );
        params.putAll( getErrorInfo( error ) );
        sendUnrealEvent( name, params );
    }

    @Override
    public void onAdHidden(final MaxAd ad)
    {
        // BMLs do not support [HIDDEN] events
        final MaxAdFormat adFormat = ad.getFormat();
        if ( adFormat != MaxAdFormat.INTERSTITIAL && adFormat != MaxAdFormat.REWARDED ) return;

        String name;
        if ( MaxAdFormat.INTERSTITIAL == adFormat )
        {
            name = "OnInterstitialHiddenEvent";
        }
        else // REWARDED
        {
            name = "OnRewardedAdHiddenEvent";
        }

        sendUnrealEvent( name, getAdInfo( ad ) );
    }

    @Override
    public void onAdExpanded(final MaxAd ad)
    {
        final MaxAdFormat adFormat = ad.getFormat();
        if ( adFormat != MaxAdFormat.BANNER && adFormat != MaxAdFormat.LEADER && adFormat != MaxAdFormat.MREC )
        {
            logInvalidAdFormat( adFormat );
            return;
        }

        sendUnrealEvent( ( MaxAdFormat.MREC == adFormat ) ? "OnMrecAdCollapsedEvent" : "OnBannerAdExpandedEvent", getAdInfo( ad ) );
    }

    @Override
    public void onAdCollapsed(final MaxAd ad)
    {
        final MaxAdFormat adFormat = ad.getFormat();
        if ( adFormat != MaxAdFormat.BANNER && adFormat != MaxAdFormat.LEADER && adFormat != MaxAdFormat.MREC )
        {
            logInvalidAdFormat( adFormat );
            return;
        }

        sendUnrealEvent( ( MaxAdFormat.MREC == adFormat ) ? "OnMRecAdCollapsedEvent" : "OnBannerAdCollapsedEvent", getAdInfo( ad ) );
    }

    @Override
    public void onRewardedVideoCompleted(final MaxAd ad)
    {
        // This event is not forwarded
    }

    @Override
    public void onRewardedVideoStarted(final MaxAd ad)
    {
        // This event is not forwarded
    }

    @Override
    public void onUserRewarded(final MaxAd ad, final MaxReward reward)
    {
        final MaxAdFormat adFormat = ad.getFormat();
        if ( adFormat != MaxAdFormat.REWARDED )
        {
            logInvalidAdFormat( adFormat );
            return;
        }

        final String rewardLabel = reward != null ? reward.getLabel() : "";
        final int rewardAmount = reward != null ? reward.getAmount() : 0;

        Map<String, String> params = getAdInfo( ad );
        params.put( "rewardLabel", rewardLabel );
        params.put( "rewardAmount", String.valueOf( rewardAmount ) );
        sendUnrealEvent( "OnRewardedAdReceivedRewardEvent", params );
    }

    @Override
    public void onAdRevenuePaid(final MaxAd ad)
    {
        final MaxAdFormat adFormat = ad.getFormat();
        final String name;
        if ( MaxAdFormat.BANNER == adFormat || MaxAdFormat.LEADER == adFormat )
        {
            name = "OnBannerAdRevenuePaidEvent";
        }
        else if ( MaxAdFormat.MREC == adFormat )
        {
            name = "OnMRecAdRevenuePaidEvent";
        }
        else if ( MaxAdFormat.CROSS_PROMO == adFormat )
        {
            name = "OnCrossPromoAdRevenuePaidEvent";
        }
        else if ( MaxAdFormat.INTERSTITIAL == adFormat )
        {
            name = "OnInterstitialAdRevenuePaidEvent";
        }
        else if ( MaxAdFormat.REWARDED == adFormat )
        {
            name = "OnRewardedAdRevenuePaidEvent";
        }
        else
        {
            logInvalidAdFormat( adFormat );
            return;
        }

        sendUnrealEvent( name, getAdInfo( ad ) );
    }

    // region Internal Methods
    private void createAdView(final String adUnitId, final MaxAdFormat adFormat, final String adViewPosition)
    {
        // Run on main thread to ensure there are no concurrency issues with other ad view methods
        getGameActivity().runOnUiThread( new Runnable()
        {
            @Override
            public void run()
            {
                d( "Creating " + adFormat.getLabel() + " with ad unit id \"" + adUnitId + "\" and position: \"" + adViewPosition + "\"" );

                // Retrieve ad view from the map
                final MaxAdView adView = retrieveAdView( adUnitId, adFormat, adViewPosition );
                if ( adView == null )
                {
                    e( adFormat.getLabel() + " does not exist" );
                    return;
                }

                adView.setVisibility( View.GONE );

                if ( adView.getParent() == null )
                {
                    final Activity currentActivity = getGameActivity();
                    final RelativeLayout relativeLayout = new RelativeLayout( currentActivity );
                    currentActivity.addContentView( relativeLayout, new LinearLayout.LayoutParams( LinearLayout.LayoutParams.MATCH_PARENT,
                                                                                                   LinearLayout.LayoutParams.MATCH_PARENT ) );
                    relativeLayout.addView( adView );

                    // Position ad view immediately so if publisher sets color before ad loads, it will not be the size of the screen
                    mAdViewAdFormats.put( adUnitId, adFormat );
                    positionAdView( adUnitId, adFormat );
                }

                adView.loadAd();

                // The publisher may have requested to show the banner before it was created. Now that the banner is created, show it.
                if ( mAdUnitIdsToShowAfterCreate.contains( adUnitId ) )
                {
                    showAdView( adUnitId, adFormat );
                    mAdUnitIdsToShowAfterCreate.remove( adUnitId );
                }
            }
        } );
    }

    private void setAdViewPlacement(final String adUnitId, final MaxAdFormat adFormat, final String placement)
    {
        getGameActivity().runOnUiThread( new Runnable()
        {
            @Override
            public void run()
            {
                d( "Setting placement \"" + placement + "\" for " + adFormat.getLabel() + " with ad unit id \"" + adUnitId + "\"" );

                final MaxAdView adView = retrieveAdView( adUnitId, adFormat );
                if ( adView == null )
                {
                    e( adFormat.getLabel() + " does not exist" );
                    return;
                }

                adView.setPlacement( placement );
            }
        } );
    }

    private void updateAdViewPosition(final String adUnitId, final String adViewPosition, final MaxAdFormat adFormat)
    {
        getGameActivity().runOnUiThread( new Runnable()
        {
            @Override
            public void run()
            {
                d( "Updating " + adFormat.getLabel() + " position to \"" + adViewPosition + "\" for ad unit id \"" + adUnitId + "\"" );

                // Retrieve ad view from the map
                final MaxAdView adView = retrieveAdView( adUnitId, adFormat );
                if ( adView == null )
                {
                    e( adFormat.getLabel() + " does not exist" );
                    return;
                }

                // Check if the previous position is same as the new position. If so, no need to update the position again.
                final String previousPosition = mAdViewPositions.get( adUnitId );
                if ( adViewPosition == null || adViewPosition.equals( previousPosition ) ) return;

                mAdViewPositions.put( adUnitId, adViewPosition );
                positionAdView( adUnitId, adFormat );
            }
        } );
    }

    private void showAdView(final String adUnitId, final MaxAdFormat adFormat)
    {
        getGameActivity().runOnUiThread( new Runnable()
        {
            @Override
            public void run()
            {
                d( "Showing " + adFormat.getLabel() + " with ad unit id \"" + adUnitId + "\"" );

                final MaxAdView adView = retrieveAdView( adUnitId, adFormat );
                if ( adView == null )
                {
                    e( adFormat.getLabel() + " does not exist for ad unit id " + adUnitId );

                    // The adView has not yet been created. Store the ad unit ID, so that it can be displayed once the banner has been created.
                    mAdUnitIdsToShowAfterCreate.add( adUnitId );
                    return;
                }

                adView.setVisibility( View.VISIBLE );
                adView.startAutoRefresh();
            }
        } );
    }

    private void hideAdView(final String adUnitId, final MaxAdFormat adFormat)
    {
        getGameActivity().runOnUiThread( new Runnable()
        {
            @Override
            public void run()
            {
                d( "Hiding " + adFormat.getLabel() + " with ad unit id \"" + adUnitId + "\"" );
                mAdUnitIdsToShowAfterCreate.remove( adUnitId );

                final MaxAdView adView = retrieveAdView( adUnitId, adFormat );
                if ( adView == null )
                {
                    e( adFormat.getLabel() + " does not exist" );
                    return;
                }

                adView.setVisibility( View.GONE );
                adView.stopAutoRefresh();
            }
        } );
    }

    private void destroyAdView(final String adUnitId, final MaxAdFormat adFormat)
    {
        getGameActivity().runOnUiThread( new Runnable()
        {
            @Override
            public void run()
            {
                d( "Destroying " + adFormat.getLabel() + " with ad unit id \"" + adUnitId + "\"" );

                final MaxAdView adView = retrieveAdView( adUnitId, adFormat );
                if ( adView == null )
                {
                    e( adFormat.getLabel() + " does not exist" );
                    return;
                }

                final ViewParent parent = adView.getParent();
                if ( parent instanceof ViewGroup )
                {
                    ( (ViewGroup) parent ).removeView( adView );
                }

                adView.setListener( null );
                adView.destroy();

                mAdViews.remove( adUnitId );
                mAdViewAdFormats.remove( adUnitId );
                mAdViewPositions.remove( adUnitId );
                mVerticalAdViewFormats.remove( adUnitId );
            }
        } );
    }

    private void setAdViewBackgroundColor(final String adUnitId, final MaxAdFormat adFormat, final String hexColorCode)
    {
        getGameActivity().runOnUiThread( new Runnable()
        {
            @Override
            public void run()
            {
                d( "Setting " + adFormat.getLabel() + " with ad unit id \"" + adUnitId + "\" to color: " + hexColorCode );

                final MaxAdView adView = retrieveAdView( adUnitId, adFormat );
                if ( adView == null )
                {
                    e( adFormat.getLabel() + " does not exist" );
                    return;
                }

                adView.setBackgroundColor( Color.parseColor( hexColorCode ) );
            }
        } );
    }

    private void setAdViewExtraParameters(final String adUnitId, final MaxAdFormat adFormat, final String key, final String value)
    {
        getGameActivity().runOnUiThread( new Runnable()
        {
            @Override
            public void run()
            {
                d( "Setting " + adFormat.getLabel() + " extra with key: \"" + key + "\" value: " + value );

                // Retrieve ad view from the map
                final MaxAdView adView = retrieveAdView( adUnitId, adFormat );
                if ( adView == null )
                {
                    e( adFormat.getLabel() + " does not exist" );
                    return;
                }

                adView.setExtraParameter( key, value );

                // Handle local changes as needed
                if ( "force_banner".equalsIgnoreCase( key ) && MaxAdFormat.MREC != adFormat )
                {
                    final MaxAdFormat forcedAdFormat;

                    boolean shouldForceBanner = Boolean.parseBoolean( value );
                    if ( shouldForceBanner )
                    {
                        forcedAdFormat = MaxAdFormat.BANNER;
                    }
                    else
                    {
                        forcedAdFormat = getDeviceSpecificBannerAdViewAdFormat();
                    }

                    mAdViewAdFormats.put( adUnitId, forcedAdFormat );
                    positionAdView( adUnitId, forcedAdFormat );
                }
            }
        } );
    }

    private void logInvalidAdFormat(MaxAdFormat adFormat)
    {
        logStackTrace( new IllegalStateException( "invalid ad format: " + adFormat ) );
    }

    private void logStackTrace(Exception e)
    {
        e( Log.getStackTraceString( e ) );
    }

    public static void d(final String message)
    {
        final String fullMessage = "[" + TAG + "] " + message;
        Log.d( SDK_TAG, fullMessage );
    }

    public static void e(final String message)
    {
        final String fullMessage = "[" + TAG + "] " + message;
        Log.e( SDK_TAG, fullMessage );
    }

    private MaxInterstitialAd retrieveInterstitial(String adUnitId)
    {
        MaxInterstitialAd result = mInterstitials.get( adUnitId );
        if ( result == null )
        {
            result = new MaxInterstitialAd( adUnitId, sdk, getGameActivity() );
            result.setListener( this );

            mInterstitials.put( adUnitId, result );
        }

        return result;
    }

    private MaxRewardedAd retrieveRewardedAd(String adUnitId)
    {
        MaxRewardedAd result = mRewardedAds.get( adUnitId );
        if ( result == null )
        {
            result = MaxRewardedAd.getInstance( adUnitId, sdk, getGameActivity() );
            result.setListener( this );

            mRewardedAds.put( adUnitId, result );
        }

        return result;
    }

    private MaxAdView retrieveAdView(String adUnitId, MaxAdFormat adFormat)
    {
        return retrieveAdView( adUnitId, adFormat, null );
    }

    public MaxAdView retrieveAdView(String adUnitId, MaxAdFormat adFormat, String adViewPosition)
    {
        MaxAdView result = mAdViews.get( adUnitId );
        if ( result == null && adViewPosition != null )
        {
            result = new MaxAdView( adUnitId, adFormat, sdk, getGameActivity() );
            result.setListener( this );

            mAdViews.put( adUnitId, result );
            mAdViewPositions.put( adUnitId, adViewPosition );
        }

        return result;
    }

    private void positionAdView(MaxAd ad)
    {
        positionAdView( ad.getAdUnitId(), ad.getFormat() );
    }

    private void positionAdView(String adUnitId, MaxAdFormat adFormat)
    {
        final MaxAdView adView = retrieveAdView( adUnitId, adFormat );
        if ( adView == null )
        {
            e( adFormat.getLabel() + " does not exist" );
            return;
        }

        final RelativeLayout relativeLayout = (RelativeLayout) adView.getParent();
        if ( relativeLayout == null )
        {
            e( adFormat.getLabel() + "'s parent does not exist" );
            return;
        }

        // Size the ad
        final String adViewPosition = mAdViewPositions.get( adUnitId );
        final AdViewSize adViewSize = getAdViewSize( adFormat );
        final int width = AppLovinSdkUtils.dpToPx( getGameActivity(), adViewSize.widthDp );
        final int height = AppLovinSdkUtils.dpToPx( getGameActivity(), adViewSize.heightDp );

        final RelativeLayout.LayoutParams params = (RelativeLayout.LayoutParams) adView.getLayoutParams();
        params.height = height;
        adView.setLayoutParams( params );

        // Parse gravity
        int gravity = 0;

        // Reset rotation, translation and margins so that the banner can be positioned again
        adView.setRotation( 0 );
        adView.setTranslationX( 0 );
        params.setMargins( 0, 0, 0, 0 );
        mVerticalAdViewFormats.remove( adUnitId );

        if ( "centered".equalsIgnoreCase( adViewPosition ) )
        {
            gravity = Gravity.CENTER_VERTICAL | Gravity.CENTER_HORIZONTAL;
        }
        else
        {
            // Figure out vertical params
            if ( adViewPosition.contains( "top" ) )
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
                final boolean containsLeft = adViewPosition.contains( "left" );
                final boolean containsRight = adViewPosition.contains( "right" );
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
                        final Rect windowRect = new Rect();
                        relativeLayout.getWindowVisibleDisplayFrame( windowRect );

                        final int windowWidth = windowRect.width();
                        final int windowHeight = windowRect.height();
                        final int longSide = Math.max( windowWidth, windowHeight );
                        final int shortSide = Math.min( windowWidth, windowHeight );
                        final int margin = ( longSide - shortSide ) / 2;
                        params.setMargins( -margin, 0, -margin, 0 );

                        // The view is now at the center of the screen and so is it's pivot point. Move its center such that when rotated, it snaps into the vertical position we need.
                        final int translationRaw = ( windowWidth / 2 ) - ( height / 2 );
                        final int translationX = containsLeft ? -translationRaw : translationRaw;
                        adView.setTranslationX( translationX );

                        // We have the view's center in the correct position. Now rotate it to snap into place.
                        adView.setRotation( 270 );

                        // Store the ad view with format, so that it can be updated when the orientation changes.
                        mVerticalAdViewFormats.put( adUnitId, adFormat );
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

    protected static class AdViewSize
    {
        public final int widthDp;
        public final int heightDp;

        private AdViewSize(final int widthDp, final int heightDp)
        {
            this.widthDp = widthDp;
            this.heightDp = heightDp;
        }
    }

    public static AdViewSize getAdViewSize(final MaxAdFormat format)
    {
        if ( MaxAdFormat.LEADER == format )
        {
            return new AdViewSize( 728, 90 );
        }
        else if ( MaxAdFormat.BANNER == format )
        {
            return new AdViewSize( 320, 50 );
        }
        else if ( MaxAdFormat.MREC == format )
        {
            return new AdViewSize( 300, 250 );
        }
        else
        {
            throw new IllegalArgumentException( "Invalid ad format" );
        }
    }

    private Map<String, String> getAdInfo(final MaxAd ad)
    {
        Map<String, String> adInfo = new HashMap<>( 5 );
        adInfo.put( "adUnitId", ad.getAdUnitId() );
        adInfo.put( "creativeId", !TextUtils.isEmpty( ad.getCreativeId() ) ? ad.getCreativeId() : "" );
        adInfo.put( "networkName", ad.getNetworkName() );
        adInfo.put( "placement", !TextUtils.isEmpty( ad.getPlacement() ) ? ad.getPlacement() : "" );
        adInfo.put( "revenue", String.valueOf( ad.getRevenue() ) );

        return adInfo;
    }

    private Map<String, String> getErrorInfo(final MaxError error)
    {
        Map<String, String> errorInfo = new HashMap<>( 5 );
        errorInfo.put( "errorCode", String.valueOf( error.getCode() ) );
        errorInfo.put( "errorMessage", !TextUtils.isEmpty( error.getMessage() ) ? error.getMessage() : "" );
        errorInfo.put( "errorAdLoadFailureInfo", !TextUtils.isEmpty( error.getAdLoadFailureInfo() ) ? error.getAdLoadFailureInfo() : "" );

        return errorInfo;
    }

    private static String serialize(Map<String, String> map)
    {
        final StringBuilder stringBuilder = new StringBuilder( 64 );
        for ( Map.Entry<String, String> entry : map.entrySet() )
        {
            stringBuilder.append( entry.getKey() );
            stringBuilder.append( SERIALIZED_KEY_VALUE_SEPARATOR );
            stringBuilder.append( entry.getValue() );
            stringBuilder.append( SERIALIZED_KEY_VALUE_PAIR_SEPARATOR );
        }

        return stringBuilder.toString();
    }

    private static Map<String, String> deserialize(final String serialized)
    {
        if ( !TextUtils.isEmpty( serialized ) )
        {
            final String[] keyValuePairs = serialized.split( SERIALIZED_KEY_VALUE_PAIR_SEPARATOR ); // ["key-1<FS>value-1", "key-2<FS>value-2", "key-3<FS>value-3"]
            final Map<String, String> deserialized = new HashMap<>( keyValuePairs.length );

            for ( final String keyValuePair : keyValuePairs )
            {
                final String[] splitPair = keyValuePair.split( SERIALIZED_KEY_VALUE_SEPARATOR );
                if ( splitPair.length == 2 )
                {
                    String key = splitPair[0];
                    String value = splitPair[1];

                    // Store in deserialized map
                    deserialized.put( key, value );
                }
            }

            return deserialized;
        }
        else
        {
            return Collections.emptyMap();
        }
    }
    // endregion

    // region Unreal Bridge
    private void sendUnrealEvent(final String name, final Map<String, String> params)
    {
        eventListener.onReceivedEvent( name, serialize( params ) );
    }
    // endregion
}
