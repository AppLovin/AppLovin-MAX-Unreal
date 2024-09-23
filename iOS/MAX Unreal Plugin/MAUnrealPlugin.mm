//
//  MAUnrealPlugin.mm
//  MAX Unreal Plugin
//
//  Created by Ritam Sarmah on 5/9/21.
//  Copyright © 2021 AppLovin. All rights reserved.
//

#import "MAUnrealPlugin.h"

#define KEY_WINDOW [UIApplication sharedApplication].keyWindow
#define DEVICE_SPECIFIC_ADVIEW_AD_FORMAT ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPad) ? MAAdFormat.leader : MAAdFormat.banner

// Internal
@interface UIColor (ALUtils)
+ (nullable UIColor *)al_colorWithHexString:(NSString *)hexString;
@end

@interface NSNumber (ALUtils)
+ (NSNumber *)al_numberWithString:(NSString *)string;
@end

@interface NSDictionary (ALUtils)
- (BOOL)al_containsValueForKey:(id)key;
@end

@interface NSString (ALUtils)
@property (nonatomic, assign, readonly, getter=al_isValidString) BOOL al_validString;
@end

@interface MAUnrealPlugin()<MAAdRevenueDelegate, MAAdDelegate, MAAdViewAdDelegate, MARewardedAdDelegate>

// Parent Fields
@property (nonatomic, weak) ALSdk *sdk;
@property (nonatomic, strong) ALAtomicBoolean *pluginInitialized;
@property (nonatomic, strong) ALAtomicBoolean *sdkInitialized;
@property (nonatomic, strong, nullable) ALSdkConfiguration *sdkConfiguration;

// Store these values if pub sets before initializing
@property (nonatomic, strong, nullable) NSArray<NSString *> *testDeviceAdvertisingIdentifiersToSet;

// Fullscreen Ad Fields
@property (nonatomic, strong) NSMutableDictionary<NSString *, MAInterstitialAd *> *interstitials;
@property (nonatomic, strong) NSMutableDictionary<NSString *, MARewardedAd *> *rewardedAds;

// Banner Fields
@property (nonatomic, strong) NSMutableDictionary<NSString *, MAAdView *> *adViews;
@property (nonatomic, strong) NSMutableDictionary<NSString *, MAAdFormat *> *adViewAdFormats;
@property (nonatomic, strong) NSMutableDictionary<NSString *, MAAdFormat *> *verticalAdViewFormats;
@property (nonatomic, strong) NSMutableDictionary<NSString *, NSString *> *adViewPositions;
@property (nonatomic, strong) NSMutableDictionary<NSString *, NSArray<NSLayoutConstraint *> *> *adViewConstraints;
@property (nonatomic, strong) NSMutableArray<NSString *> *adUnitIdentifiersToShowAfterCreate;
@property (nonatomic, strong) UIView *safeAreaBackground;
@property (nonatomic, strong, nullable) UIColor *publisherBannerBackgroundColor;

@property (nonatomic, weak, nullable) UIView *unrealMainView;
@property (nonatomic, assign) UnrealEventCallback eventCallback;

@end

@implementation MAUnrealPlugin
static NSString *const SDK_TAG = @"AppLovinSdk";
static NSString *const TAG = @"MAUnrealPlugin";

#pragma mark - Initialization

- (instancetype)initWithView:(UIView *)mainView eventCallback:(UnrealEventCallback)eventCallback
{
    self = [super init];
    if ( self )
    {
        self.pluginInitialized = [[ALAtomicBoolean alloc] init];
        self.sdkInitialized = [[ALAtomicBoolean alloc] init];
        
        self.interstitials = [NSMutableDictionary dictionaryWithCapacity: 2];
        self.rewardedAds = [NSMutableDictionary dictionaryWithCapacity: 2];
        self.adViews = [NSMutableDictionary dictionaryWithCapacity: 2];
        self.adViewAdFormats = [NSMutableDictionary dictionaryWithCapacity: 2];
        self.verticalAdViewFormats = [NSMutableDictionary dictionaryWithCapacity: 2];
        self.adViewPositions = [NSMutableDictionary dictionaryWithCapacity: 2];
        self.adViewConstraints = [NSMutableDictionary dictionaryWithCapacity: 2];
        self.adUnitIdentifiersToShowAfterCreate = [NSMutableArray arrayWithCapacity: 2];
        
        self.unrealMainView = mainView;
        self.eventCallback = eventCallback;
        
        self.sdk = [ALSdk shared];
        
        dispatchOnMainQueue(^{
            
            self.safeAreaBackground = [[UIView alloc] init];
            self.safeAreaBackground.hidden = YES;
            self.safeAreaBackground.backgroundColor = UIColor.clearColor;
            self.safeAreaBackground.translatesAutoresizingMaskIntoConstraints = NO;
            self.safeAreaBackground.userInteractionEnabled = NO;
            
            [self.unrealMainView addSubview: self.safeAreaBackground];
        });
    }
    return self;
}

- (void)initialize:(NSString *)pluginVersion sdkKey:(NSString *)sdkKey
{
    // Guard against running init logic multiple times
    if ( [self.pluginInitialized compareAndSet: NO update: YES] )
    {
        [self sendInitializationEvent];
        return;
    }
    
    [self d: @"Initializing AppLovin MAX Unreal v%@...", pluginVersion];
    
    if ( ![sdkKey al_isValidString] )
    {
        [NSException raise: NSInternalInconsistencyException
                    format: @"Unable to initialize AppLovin MAX Unreal Plugin - no SDK key provided"];
    }
    
    // Create initialization configuration
    ALSdkInitializationConfiguration *initConfiguration = [ALSdkInitializationConfiguration configurationWithSdkKey: sdkKey
                                                                                                       builderBlock:^(ALSdkInitializationConfigurationBuilder *builder) {
        builder.mediationProvider = ALMediationProviderMAX;
        builder.pluginVersion = [@"Unreal-" stringByAppendingString: pluginVersion];
        
        if ( !self.testDeviceAdvertisingIdentifiersToSet )
        {
            builder.testDeviceAdvertisingIdentifiers = self.testDeviceAdvertisingIdentifiersToSet;
        }
    }];
    
    // Initialize SDK
    [self.sdk initializeWithConfiguration: initConfiguration completionHandler:^(ALSdkConfiguration *sdkConfiguration) {
        
        self.sdkConfiguration = sdkConfiguration;
        [self.sdkInitialized set: YES];
        
        // Enable orientation change listener, so that the position can be updated for vertical banners.
        [[NSNotificationCenter defaultCenter] addObserverForName: UIDeviceOrientationDidChangeNotification
                                                          object: nil
                                                           queue: [NSOperationQueue mainQueue]
                                                      usingBlock:^(NSNotification *notification) {
            
            for ( NSString *adUnitIdentifier in self.verticalAdViewFormats )
            {
                [self positionAdViewForAdUnitIdentifier: adUnitIdentifier adFormat: self.verticalAdViewFormats[adUnitIdentifier]];
            }
        }];

        [self sendInitializationEvent];
    }];
}

- (void)sendInitializationEvent
{
    NSMutableDictionary<NSString *, id> *parameters = [NSMutableDictionary dictionaryWithCapacity: 6];
    
    if ( self.sdkConfiguration )
    {
        // Unreal blueprints only support uint8 enums. Since ALAppTrackingTransparencyStatusUnavailable = -1,
        // increment app tracking status values by 1 to make them all unsigned
        parameters[@"appTrackingStatus"] = @(self.sdkConfiguration.appTrackingTransparencyStatus + 1);
        parameters[@"consentFlowUserGeography"] = @(self.sdkConfiguration.consentFlowUserGeography);
        parameters[@"countryCode"] = self.sdkConfiguration.countryCode;
    }
    
    parameters[@"hasUserConsent"] = @([ALPrivacySettings hasUserConsent]);
    parameters[@"isDoNotSell"] = @([ALPrivacySettings isDoNotSell]);
    parameters[@"isTablet"] = @([self isTablet]);
    
    [self sendUnrealEventWithName: @"OnSdkInitializedEvent" parameters: parameters];
}

- (BOOL)isInitialized
{
    return [self.pluginInitialized get] && [self.sdkInitialized get];
}

#pragma mark - Privacy

- (void)setHasUserConsent:(BOOL)hasUserConsent
{
    [ALPrivacySettings setHasUserConsent: hasUserConsent];
}

- (BOOL)hasUserConsent
{
    return [ALPrivacySettings hasUserConsent];
}

- (void)setDoNotSell:(BOOL)doNotSell
{
    [ALPrivacySettings setDoNotSell: doNotSell];
}

- (BOOL)isDoNotSell
{
    return [ALPrivacySettings isDoNotSell];
}

#pragma mark - Terms and Privacy Policy Flow

- (void)setTermsAndPrivacyPolicyFlowEnabled:(BOOL)enabled
{
    self.sdk.settings.termsAndPrivacyPolicyFlowSettings.enabled = enabled;
}

- (void)setPrivacyPolicyURL:(NSString *)urlString
{
    self.sdk.settings.termsAndPrivacyPolicyFlowSettings.privacyPolicyURL = [NSURL URLWithString: urlString];
}

- (void)setTermsOfServiceURL:(NSString *)urlString
{
    self.sdk.settings.termsAndPrivacyPolicyFlowSettings.termsOfServiceURL = [NSURL URLWithString: urlString];
}

- (void)setConsentFlowDebugUserGeography:(NSString *)userGeographyString
{
    self.sdk.settings.termsAndPrivacyPolicyFlowSettings.debugUserGeography = [self userGeographyForString: userGeographyString];
}

- (void)showCMPForExistingUser
{
    if ( ![self isInitialized] )
    {
        [self logUninitializedError: @"Failed to show CMP flow"];
        return;
    }
        
    [self.sdk.cmpService showCMPForExistingUserWithCompletion:^(ALCMPError * _Nullable error) {
        
        NSDictionary<NSString *, id> *parameters = @{};
        if ( error )
        {
            parameters = @{@"code" : @(error.code),
                           @"message" : error.message ?: @"",
                           @"cmpCode" : @(error.cmpCode),
                           @"cmpMessage" : error.cmpMessage ?: @""};
        }
        
        [self sendUnrealEventWithName: @"OnCmpCompletedEvent" parameters: parameters];
    }];
}

- (BOOL)hasSupportedCMP
{
    return [self.sdk.cmpService hasSupportedCMP];
}

#pragma mark - General

- (BOOL)isTablet
{
    return [UIDevice currentDevice].userInterfaceIdiom == UIUserInterfaceIdiomPad;
}

- (void)showMediationDebugger
{
    if ( ![self isInitialized] )
    {
        [self logUninitializedError: @"Failed to show mediation debugger"];
        return;
    }
    
    [self.sdk showMediationDebugger];
}

- (void)setUserId:(NSString *)userId
{
    self.sdk.settings.userIdentifier = userId;
}

- (void)setMuted:(BOOL)muted
{
    self.sdk.settings.muted = muted;
}

- (BOOL)isMuted
{
    return self.sdk.settings.muted;
}

- (void)setVerboseLoggingEnabled:(BOOL)enabled
{
    self.sdk.settings.verboseLoggingEnabled = enabled;
}

- (BOOL)isVerboseLoggingEnabled
{
    return [self.sdk.settings isVerboseLoggingEnabled];
}

- (void)setCreativeDebuggerEnabled:(BOOL)enabled
{
    self.sdk.settings.creativeDebuggerEnabled = enabled;
}

- (void)setTestDeviceAdvertisingIds:(NSArray<NSString *> *)testDeviceAdvertisingIds
{
    if ( [self isInitialized] )
    {
        [self e: @"Failed to set test device advertising IDs. Please set your test device advertising IDs before initializing the plugin."];
        return;
    }

    self.testDeviceAdvertisingIdentifiersToSet = testDeviceAdvertisingIds;
}

#pragma mark - Event Tracking

- (void)trackEvent:(NSString *)event parameters:(NSDictionary<NSString *, NSString *> *)parameters
{
    if ( ![self isInitialized] )
    {
        NSString *message = [NSString stringWithFormat: @"Failed to track event (%@)", event];
        [self logUninitializedError: message];
        return;
    }
    
    [self.sdk.eventService trackEvent: event parameters: parameters];
}

#pragma mark - Banners

- (void)createBannerWithAdUnitIdentifier:(NSString *)adUnitIdentifier atPosition:(NSString *)bannerPosition
{
    [self createAdViewWithAdUnitIdentifier: adUnitIdentifier adFormat: DEVICE_SPECIFIC_ADVIEW_AD_FORMAT atPosition: bannerPosition];
}

- (void)setBannerBackgroundColorForAdUnitIdentifier:(NSString *)adUnitIdentifier hexColorCode:(NSString *)hexColorCode
{
    [self setAdViewBackgroundColorForAdUnitIdentifier: adUnitIdentifier adFormat: DEVICE_SPECIFIC_ADVIEW_AD_FORMAT hexColorCode: hexColorCode];
}

- (void)setBannerPlacement:(nullable NSString *)placement forAdUnitIdentifier:(NSString *)adUnitIdentifier
{
    [self setAdViewPlacement: placement forAdUnitIdentifier: adUnitIdentifier adFormat: DEVICE_SPECIFIC_ADVIEW_AD_FORMAT];
}

- (void)setBannerExtraParameterForAdUnitIdentifier:(NSString *)adUnitIdentifier key:(NSString *)key value:(nullable NSString *)value
{
    [self setAdViewExtraParameterForAdUnitIdentifier: adUnitIdentifier adFormat: DEVICE_SPECIFIC_ADVIEW_AD_FORMAT key: key value: value];
}

- (void)updateBannerPosition:(NSString *)bannerPosition forAdUnitIdentifier:(NSString *)adUnitIdentifier
{
    [self updateAdViewPosition: bannerPosition forAdUnitIdentifier: adUnitIdentifier adFormat: DEVICE_SPECIFIC_ADVIEW_AD_FORMAT];
}

- (void)showBannerWithAdUnitIdentifier:(NSString *)adUnitIdentifier
{
    [self showAdViewWithAdUnitIdentifier: adUnitIdentifier adFormat: DEVICE_SPECIFIC_ADVIEW_AD_FORMAT];
}

- (void)hideBannerWithAdUnitIdentifier:(NSString *)adUnitIdentifier
{
    [self hideAdViewWithAdUnitIdentifier: adUnitIdentifier adFormat: DEVICE_SPECIFIC_ADVIEW_AD_FORMAT];
}

- (void)destroyBannerWithAdUnitIdentifier:(NSString *)adUnitIdentifier
{
    [self destroyAdViewWithAdUnitIdentifier: adUnitIdentifier adFormat: DEVICE_SPECIFIC_ADVIEW_AD_FORMAT];
}

#pragma mark - MRECs

- (void)createMRecWithAdUnitIdentifier:(NSString *)adUnitIdentifier atPosition:(NSString *)mrecPosition
{
    [self createAdViewWithAdUnitIdentifier: adUnitIdentifier adFormat: MAAdFormat.mrec atPosition: mrecPosition];
}

- (void)setMRecPlacement:(nullable NSString *)placement forAdUnitIdentifier:(NSString *)adUnitIdentifier
{
    [self setAdViewPlacement: placement forAdUnitIdentifier: adUnitIdentifier adFormat: MAAdFormat.mrec];
}

- (void)setMRecExtraParameterForAdUnitIdentifier:(NSString *)adUnitIdentifier key:(NSString *)key value:(nullable NSString *)value
{
    [self setAdViewExtraParameterForAdUnitIdentifier: adUnitIdentifier adFormat: MAAdFormat.mrec key: key value: value];
}

- (void)updateMRecPosition:(NSString *)mrecPosition forAdUnitIdentifier:(NSString *)adUnitIdentifier
{
    [self updateAdViewPosition: mrecPosition forAdUnitIdentifier: adUnitIdentifier adFormat: MAAdFormat.mrec];
}

- (void)showMRecWithAdUnitIdentifier:(NSString *)adUnitIdentifier
{
    [self showAdViewWithAdUnitIdentifier: adUnitIdentifier adFormat: MAAdFormat.mrec];
}

- (void)hideMRecWithAdUnitIdentifier:(NSString *)adUnitIdentifier
{
    [self hideAdViewWithAdUnitIdentifier: adUnitIdentifier adFormat: MAAdFormat.mrec];
}

- (void)destroyMRecWithAdUnitIdentifier:(NSString *)adUnitIdentifier
{
    [self destroyAdViewWithAdUnitIdentifier: adUnitIdentifier adFormat: MAAdFormat.mrec];
}

#pragma mark - Interstitials

- (void)loadInterstitialWithAdUnitIdentifier:(NSString *)adUnitIdentifier
{
    MAInterstitialAd *interstitial = [self retrieveInterstitialForAdUnitIdentifier: adUnitIdentifier];
    [interstitial loadAd];
}

- (BOOL)isInterstitialReadyWithAdUnitIdentifier:(NSString *)adUnitIdentifier
{
    MAInterstitialAd *interstitial = [self retrieveInterstitialForAdUnitIdentifier: adUnitIdentifier];
    return [interstitial isReady];
}

- (void)showInterstitialWithAdUnitIdentifier:(NSString *)adUnitIdentifier placement:(NSString *)placement
{
    MAInterstitialAd *interstitial = [self retrieveInterstitialForAdUnitIdentifier: adUnitIdentifier];
    [interstitial showAdForPlacement: placement];
}

- (void)setInterstitialExtraParameterForAdUnitIdentifier:(NSString *)adUnitIdentifier key:(NSString *)key value:(NSString *)value
{
    MAInterstitialAd *interstitial = [self retrieveInterstitialForAdUnitIdentifier: adUnitIdentifier];
    [interstitial setExtraParameterForKey: key value: value];
}

#pragma mark - Rewarded

- (void)loadRewardedAdWithAdUnitIdentifier:(NSString *)adUnitIdentifier
{
    MARewardedAd *rewardedAd = [self retrieveRewardedAdForAdUnitIdentifier: adUnitIdentifier];
    [rewardedAd loadAd];
}

- (BOOL)isRewardedAdReadyWithAdUnitIdentifier:(NSString *)adUnitIdentifier
{
    MARewardedAd *rewardedAd = [self retrieveRewardedAdForAdUnitIdentifier: adUnitIdentifier];
    return [rewardedAd isReady];
}

- (void)showRewardedAdWithAdUnitIdentifier:(NSString *)adUnitIdentifier placement:(NSString *)placement
{
    MARewardedAd *rewardedAd = [self retrieveRewardedAdForAdUnitIdentifier: adUnitIdentifier];
    [rewardedAd showAdForPlacement: placement];
}

- (void)setRewardedAdExtraParameterForAdUnitIdentifier:(NSString *)adUnitIdentifier key:(NSString *)key value:(nullable NSString *)value
{
    MARewardedAd *rewardedAd = [self retrieveRewardedAdForAdUnitIdentifier: adUnitIdentifier];
    [rewardedAd setExtraParameterForKey: key value: value];
}

#pragma mark - Ad Callbacks

- (void)didLoadAd:(MAAd *)ad
{
    MAAdFormat *adFormat = ad.format;
    if ( [self isInvalidAdFormat: adFormat] )
    {
        [self logInvalidAdFormat: adFormat];
        return;
    }
    
    if ( [adFormat isAdViewAd] )
    {
        MAAdView *adView = [self retrieveAdViewForAdUnitIdentifier: ad.adUnitIdentifier adFormat: adFormat];
        adView.userInteractionEnabled = YES; // An ad is now being shown, enable user interaction.
        
        [self positionAdViewForAd: ad];
        
        // Do not auto-refresh by default if the ad view is not showing yet (e.g. first load during app launch and publisher does not automatically show banner upon load success)
        // We will resume auto-refresh in -[MAUnrealPlugin showBannerWithAdUnitIdentifier:].
        if ( adView && [adView isHidden] )
        {
            [adView stopAutoRefresh];
        }
    }
    
    NSString *name = [self eventNameForAdFormat: adFormat event: @"Loaded"];
    [self sendUnrealEventWithName: name parameters: [self adInfoForAd: ad]];
}

- (void)didFailToLoadAdForAdUnitIdentifier:(NSString *)adUnitIdentifier withError:(MAError *)error
{
    // Determine ad format
    MAAdFormat *adFormat;
    if ( [self.adViews al_containsValueForKey: adUnitIdentifier] )
    {
        adFormat = self.adViewAdFormats[adUnitIdentifier];
    }
    else if ( [self.interstitials al_containsValueForKey: adUnitIdentifier] )
    {
        adFormat = MAAdFormat.interstitial;
    }
    else if ( [self.rewardedAds al_containsValueForKey: adUnitIdentifier] )
    {
        adFormat = MAAdFormat.rewarded;
    }
    else
    {
        NSString *message = [NSString stringWithFormat: @"Invalid ad unit ID: %@", adUnitIdentifier];
        [self logStackTrace: message];
        return;
    }
    
    NSString *name = [self eventNameForAdFormat: adFormat event: @"LoadFailed"];
    NSMutableDictionary *parameters = [[self errorInfoForError: error] mutableCopy];
    parameters[@"adUnitIdentifier"] = adUnitIdentifier;
    
    [self sendUnrealEventWithName: name parameters: parameters];
}

- (void)didClickAd:(MAAd *)ad
{
    MAAdFormat *adFormat = ad.format;
    if ( [self isInvalidAdFormat: adFormat] )
    {
        [self logInvalidAdFormat: adFormat];
        return;
    }
    
    NSString *name = [self eventNameForAdFormat: adFormat event: @"Clicked"];
    [self sendUnrealEventWithName: name parameters: [self adInfoForAd: ad]];
}

- (void)didDisplayAd:(MAAd *)ad
{
    // BMLs do not support [DISPLAY] events
    MAAdFormat *adFormat = ad.format;
    if ( [adFormat isAdViewAd] )
    {
        [self logInvalidAdFormat: adFormat];
        return;
    }
    
    NSString *name = [self eventNameForAdFormat: adFormat event: @"Displayed"];
    [self sendUnrealEventWithName: name parameters: [self adInfoForAd: ad]];
}

- (void)didFailToDisplayAd:(MAAd *)ad withError:(MAError *)error
{
    // BMLs do not support [DISPLAY] events
    MAAdFormat *adFormat = ad.format;
    if ( [adFormat isAdViewAd] )
    {
        [self logInvalidAdFormat: adFormat];
        return;
    }
    
    NSMutableDictionary *parameters = [[self adInfoForAd: ad] mutableCopy];
    [parameters addEntriesFromDictionary: [self errorInfoForError: error]];
    
    NSString *name = [self eventNameForAdFormat: adFormat event: @"DisplayFailed"];
    [self sendUnrealEventWithName: name parameters: parameters];
}

- (void)didHideAd:(MAAd *)ad
{
    // BMLs do not support [HIDDEN] events
    MAAdFormat *adFormat = ad.format;
    if ( [adFormat isAdViewAd] )
    {
        [self logInvalidAdFormat: adFormat];
        return;
    }
    
    NSString *name = [self eventNameForAdFormat: adFormat event: @"Hidden"];
    [self sendUnrealEventWithName: name parameters: [self adInfoForAd: ad]];
}

- (void)didExpandAd:(MAAd *)ad
{
    MAAdFormat *adFormat = ad.format;
    if ( ![adFormat isAdViewAd] )
    {
        [self logInvalidAdFormat: adFormat];
        return;
    }
    
    NSString *name = [self eventNameForAdFormat: adFormat event: @"Expanded"];
    [self sendUnrealEventWithName: name parameters: [self adInfoForAd: ad]];
}

- (void)didCollapseAd:(MAAd *)ad
{
    MAAdFormat *adFormat = ad.format;
    if ( ![adFormat isAdViewAd] )
    {
        [self logInvalidAdFormat: adFormat];
        return;
    }
    
    NSString *name = [self eventNameForAdFormat: adFormat event: @"Collapsed"];
    [self sendUnrealEventWithName: name parameters: [self adInfoForAd: ad]];
}

- (void)didRewardUserForAd:(MAAd *)ad withReward:(MAReward *)reward
{
    MAAdFormat *adFormat = ad.format;
    if ( adFormat != MAAdFormat.rewarded )
    {
        [self logInvalidAdFormat: adFormat];
        return;
    }
    
    NSMutableDictionary *parameters = [[self adInfoForAd: ad] mutableCopy];
    parameters[@"label"] = reward.label;
    parameters[@"amount"] = @(reward.amount);
    
    NSString *name = [self eventNameForAdFormat: adFormat event: @"ReceivedReward"];
    [self sendUnrealEventWithName: name parameters: parameters];
}

- (void)didPayRevenueForAd:(MAAd *)ad
{
    MAAdFormat *adFormat = ad.format;
    if ( [self isInvalidAdFormat: adFormat] )
    {
        [self logInvalidAdFormat: adFormat];
        return;
    }
    
    NSString *name = [self eventNameForAdFormat: adFormat event: @"AdRevenuePaid"];
    [self sendUnrealEventWithName: name parameters: [self adInfoForAd: ad]];
}

#pragma mark - Internal Methods

- (void)createAdViewWithAdUnitIdentifier:(NSString *)adUnitIdentifier adFormat:(MAAdFormat *)adFormat atPosition:(NSString *)adViewPosition
{
    dispatchOnMainQueue(^{
        
        [self d: @"Creating %@ with ad unit identifier \"%@\" and position: \"%@\"", adFormat.label, adUnitIdentifier, adViewPosition];
        
        // Retrieve ad view from the map
        MAAdView *adView = [self retrieveAdViewForAdUnitIdentifier: adUnitIdentifier adFormat: adFormat atPosition: adViewPosition];
        if ( !adView )
        {
            [self logInvalidAdUnitIdentifier: adUnitIdentifier forAdFormat: adFormat];
            return;
        }
        
        adView.hidden = YES;
        self.safeAreaBackground.hidden = YES;
        
        // Position ad view immediately so if publisher sets color before ad loads, it will not be the size of the screen
        self.adViewAdFormats[adUnitIdentifier] = adFormat;
        [self positionAdViewForAdUnitIdentifier: adUnitIdentifier adFormat: adFormat];
        
        [adView loadAd];
        
        // The publisher may have requested to show the banner before it was created. Now that the banner is created, show it.
        if ( [self.adUnitIdentifiersToShowAfterCreate containsObject: adUnitIdentifier] )
        {
            [self showAdViewWithAdUnitIdentifier: adUnitIdentifier adFormat: adFormat];
            [self.adUnitIdentifiersToShowAfterCreate removeObject: adUnitIdentifier];
        }
    });
}

- (void)setAdViewBackgroundColorForAdUnitIdentifier:(NSString *)adUnitIdentifier adFormat:(MAAdFormat *)adFormat hexColorCode:(NSString *)hexColorCode
{
    dispatchOnMainQueue(^{
        
        [self d: @"Setting %@ with ad unit identifier \"%@\" to color: \"%@\"", adFormat.label, adUnitIdentifier, hexColorCode];
        
        MAAdView *adView = [self retrieveAdViewForAdUnitIdentifier: adUnitIdentifier adFormat: adFormat];
        if ( !adView )
        {
            [self logInvalidAdUnitIdentifier: adUnitIdentifier forAdFormat: adFormat];
            return;
        }
        
        // In some cases, black color may get redrawn on each frame update, resulting in an undesired flicker
        NSString *hexColorCodeToUse = [hexColorCode containsString: @"FF000000"] ? @"FF000001" : hexColorCode;
        UIColor *convertedColor = [UIColor al_colorWithHexString: hexColorCodeToUse];

        self.publisherBannerBackgroundColor = convertedColor;
        self.safeAreaBackground.backgroundColor = adView.backgroundColor = convertedColor;
    });
}

- (void)setAdViewPlacement:(nullable NSString *)placement forAdUnitIdentifier:(NSString *)adUnitIdentifier adFormat:(MAAdFormat *)adFormat
{
    dispatchOnMainQueue(^{
        
        [self d: @"Setting placement \"%@\" for \"%@\" with ad unit identifier \"%@\"", placement, adFormat.label, adUnitIdentifier];

        MAAdView *adView = [self retrieveAdViewForAdUnitIdentifier: adUnitIdentifier adFormat: adFormat];
        if ( !adView )
        {
            [self logInvalidAdUnitIdentifier: adUnitIdentifier forAdFormat: adFormat];
            return;
        }
        
        adView.placement = placement;
    });
}

- (void)setAdViewExtraParameterForAdUnitIdentifier:(NSString *)adUnitIdentifier adFormat:(MAAdFormat *)adFormat key:(NSString *)key value:(nullable NSString *)value
{
    dispatchOnMainQueue(^{
        
        [self d: @"Setting %@ extra with key: \"%@\" value: \"%@\"", adFormat.label, key, value];
        
        MAAdView *adView = [self retrieveAdViewForAdUnitIdentifier: adUnitIdentifier adFormat: adFormat];
        if ( !adView )
        {
            [self logInvalidAdUnitIdentifier: adUnitIdentifier forAdFormat: adFormat];
            return;
        }

        [adView setExtraParameterForKey: key value: value];
        
        // Handle local changes as needed
        if ( [@"force_banner" al_isEqualToStringIgnoringCase: key] && MAAdFormat.mrec != adFormat )
        {
            BOOL shouldForceBanner = [NSNumber al_numberWithString: value].boolValue;
            MAAdFormat *forcedAdFormat = shouldForceBanner ? MAAdFormat.banner : DEVICE_SPECIFIC_ADVIEW_AD_FORMAT;
            
            self.adViewAdFormats[adUnitIdentifier] = forcedAdFormat;
            [self positionAdViewForAdUnitIdentifier: adUnitIdentifier adFormat: forcedAdFormat];
        }
    });
}

- (void)updateAdViewPosition:(NSString *)adViewPosition forAdUnitIdentifier:(NSString *)adUnitIdentifier adFormat:(MAAdFormat *)adFormat
{
    dispatchOnMainQueue(^{

        [self d: @"Updating %@ position to \"%@\" for ad unit identifier \"%@\"", adFormat.label, adViewPosition, adUnitIdentifier];

        MAAdView *adView = [self retrieveAdViewForAdUnitIdentifier: adUnitIdentifier adFormat: adFormat];
        if ( !adView )
        {
            [self logInvalidAdUnitIdentifier: adUnitIdentifier forAdFormat: adFormat];
            return;
        }
        
        // Check if the previous position is same as the new position. If so, no need to update the position again.
        NSString *previousPosition = self.adViewPositions[adUnitIdentifier];
        if ( !adViewPosition || [adViewPosition isEqualToString: previousPosition] ) return;
        
        self.adViewPositions[adUnitIdentifier] = adViewPosition;
        [self positionAdViewForAdUnitIdentifier: adUnitIdentifier adFormat: adFormat];
    });
}

- (void)showAdViewWithAdUnitIdentifier:(NSString *)adUnitIdentifier adFormat:(MAAdFormat *)adFormat
{
    dispatchOnMainQueue(^{
        
        [self d: @"Showing %@ with ad unit identifier \"%@\"", adFormat.label, adUnitIdentifier];
        
        MAAdView *adView = [self retrieveAdViewForAdUnitIdentifier: adUnitIdentifier adFormat: adFormat];
        if ( !adView )
        {
            [self logInvalidAdUnitIdentifier: adUnitIdentifier forAdFormat: adFormat];            

            // The adView has not yet been created. Store the ad unit ID, so that it can be displayed once the banner has been created.
            [self.adUnitIdentifiersToShowAfterCreate addObject: adUnitIdentifier];
            return;
        }
        
        self.safeAreaBackground.hidden = NO;
        adView.hidden = NO;
        [adView startAutoRefresh];
    });
}

- (void)hideAdViewWithAdUnitIdentifier:(NSString *)adUnitIdentifier adFormat:(MAAdFormat *)adFormat
{
    dispatchOnMainQueue(^{
        
        [self d: @"Hiding %@ with ad unit identifier \"%@\"", adFormat.label, adUnitIdentifier];
        [self.adUnitIdentifiersToShowAfterCreate removeObject: adUnitIdentifier];

        MAAdView *adView = [self retrieveAdViewForAdUnitIdentifier: adUnitIdentifier adFormat: adFormat];
        if ( !adView )
        {
            [self logInvalidAdUnitIdentifier: adUnitIdentifier forAdFormat: adFormat];
            return;
        }
        
        adView.hidden = YES;
        self.safeAreaBackground.hidden = YES;
        
        [adView stopAutoRefresh];
    });
}

- (void)destroyAdViewWithAdUnitIdentifier:(NSString *)adUnitIdentifier adFormat:(MAAdFormat *)adFormat
{
    dispatchOnMainQueue(^{
        
        [self d: @"Destroying %@ with ad unit identifier \"%@\"", adFormat.label, adUnitIdentifier];

        MAAdView *adView = [self retrieveAdViewForAdUnitIdentifier: adUnitIdentifier adFormat: adFormat];
        if ( !adView )
        {
            [self logInvalidAdUnitIdentifier: adUnitIdentifier forAdFormat: adFormat];
            return;
        }
        
        adView.delegate = nil;
        
        [adView removeFromSuperview];
        
        [self.adViews removeObjectForKey: adUnitIdentifier];
        [self.adViewAdFormats removeObjectForKey: adUnitIdentifier];
        [self.verticalAdViewFormats removeObjectForKey: adUnitIdentifier];
        [self.adViewPositions removeObjectForKey: adUnitIdentifier];
    });
}

- (MAInterstitialAd *)retrieveInterstitialForAdUnitIdentifier:(NSString *)adUnitIdentifier
{
    MAInterstitialAd *interstitial = self.interstitials[adUnitIdentifier];
    if ( !interstitial )
    {
        interstitial = [[MAInterstitialAd alloc] initWithAdUnitIdentifier: adUnitIdentifier sdk: self.sdk];
        interstitial.delegate = self;
        
        self.interstitials[adUnitIdentifier] = interstitial;
    }
    
    return interstitial;
}

- (MARewardedAd *)retrieveRewardedAdForAdUnitIdentifier:(NSString *)adUnitIdentifier
{
    MARewardedAd *rewardedAd = self.rewardedAds[adUnitIdentifier];
    if ( !rewardedAd )
    {
        rewardedAd = [MARewardedAd sharedWithAdUnitIdentifier: adUnitIdentifier sdk: self.sdk];
        rewardedAd.delegate = self;
        
        self.rewardedAds[adUnitIdentifier] = rewardedAd;
    }
    
    return rewardedAd;
}

- (MAAdView *)retrieveAdViewForAdUnitIdentifier:(NSString *)adUnitIdentifier adFormat:(MAAdFormat *)adFormat
{
    return [self retrieveAdViewForAdUnitIdentifier: adUnitIdentifier adFormat: adFormat atPosition: nil];
}

- (MAAdView *)retrieveAdViewForAdUnitIdentifier:(NSString *)adUnitIdentifier adFormat:(MAAdFormat *)adFormat atPosition:(NSString *)adViewPosition
{
    MAAdView *adView = self.adViews[adUnitIdentifier];
    if ( !adView && adViewPosition )
    {
        adView = [[MAAdView alloc] initWithAdUnitIdentifier: adUnitIdentifier adFormat: adFormat sdk: self.sdk];
        adView.delegate = self;
        adView.userInteractionEnabled = NO;
        adView.translatesAutoresizingMaskIntoConstraints = NO;
        
        self.adViews[adUnitIdentifier] = adView;
        
        self.adViewPositions[adUnitIdentifier] = adViewPosition;
        [self.unrealMainView addSubview: adView];
    }
    
    return adView;
}

- (void)positionAdViewForAd:(MAAd *)ad
{
    [self positionAdViewForAdUnitIdentifier: ad.adUnitIdentifier adFormat: ad.format];
}

- (void)positionAdViewForAdUnitIdentifier:(NSString *)adUnitIdentifier adFormat:(MAAdFormat *)adFormat
{
    MAAdView *adView = [self retrieveAdViewForAdUnitIdentifier: adUnitIdentifier adFormat: adFormat];
    if ( !adView )
    {
        [self logInvalidAdUnitIdentifier: adUnitIdentifier forAdFormat: adFormat];
        return;
    }
    
    UIView *superview = adView.superview;
    if ( !superview ) return;

    NSString *adViewPosition = self.adViewPositions[adUnitIdentifier];
    CGSize adViewSize = adFormat.size;
    
    // Deactivate any previous constraints so that the banner can be positioned again.
    NSArray<NSLayoutConstraint *> *activeConstraints = self.adViewConstraints[adUnitIdentifier];
    [NSLayoutConstraint deactivateConstraints: activeConstraints];
    adView.transform = CGAffineTransformIdentity;
    [self.verticalAdViewFormats removeObjectForKey: adUnitIdentifier];
    
    // Ensure superview contains the safe area background.
    if ( ![superview.subviews containsObject: self.safeAreaBackground] )
    {
        [self.safeAreaBackground removeFromSuperview];
        [superview insertSubview: self.safeAreaBackground belowSubview: adView];
    }
    
    // Deactivate any previous constraints and reset visibility state so that the safe area background can be positioned again.
    [NSLayoutConstraint deactivateConstraints: self.safeAreaBackground.constraints];
    self.safeAreaBackground.hidden = adView.hidden;
    
    // All positions have constant height
    NSMutableArray<NSLayoutConstraint *> *constraints = [NSMutableArray arrayWithObject: [adView.heightAnchor constraintEqualToConstant: adViewSize.height]];
    
    UILayoutGuide *layoutGuide = superview.safeAreaLayoutGuide;
    
    // If top of bottom center, stretch width of screen
    if ( [adViewPosition isEqual: @"top_center"] || [adViewPosition isEqual: @"bottom_center"] )
    {
        // If publisher actually provided a banner background color, span the banner across the realm
        if ( self.publisherBannerBackgroundColor && adFormat != MAAdFormat.mrec )
        {
            [constraints addObjectsFromArray: @[[self.safeAreaBackground.leftAnchor constraintEqualToAnchor: superview.leftAnchor],
                                                [self.safeAreaBackground.rightAnchor constraintEqualToAnchor: superview.rightAnchor]]];
            
            if ( [adViewPosition isEqual: @"top_center"] )
            {
                [constraints addObjectsFromArray: @[[adView.topAnchor constraintEqualToAnchor: layoutGuide.topAnchor],
                                                    [adView.leftAnchor constraintEqualToAnchor: superview.leftAnchor],
                                                    [adView.rightAnchor constraintEqualToAnchor: superview.rightAnchor]]];
                [constraints addObjectsFromArray: @[[self.safeAreaBackground.topAnchor constraintEqualToAnchor: superview.topAnchor],
                                                    [self.safeAreaBackground.bottomAnchor constraintEqualToAnchor: adView.topAnchor]]];
            }
            else // BottomCenter
            {
                [constraints addObjectsFromArray: @[[adView.bottomAnchor constraintEqualToAnchor: layoutGuide.bottomAnchor],
                                                    [adView.leftAnchor constraintEqualToAnchor: superview.leftAnchor],
                                                    [adView.rightAnchor constraintEqualToAnchor: superview.rightAnchor]]];
                [constraints addObjectsFromArray: @[[self.safeAreaBackground.topAnchor constraintEqualToAnchor: adView.bottomAnchor],
                                                    [self.safeAreaBackground.bottomAnchor constraintEqualToAnchor: superview.bottomAnchor]]];
            }
        }
        // If pub does not have a background color set - we shouldn't span the banner the width of the realm (there might be user-interactable UI on the sides)
        else
        {
            self.safeAreaBackground.hidden = YES;
            
            // Assign constant width of 320 or 728
            [constraints addObject: [adView.widthAnchor constraintEqualToConstant: adViewSize.width]];
            [constraints addObject: [adView.centerXAnchor constraintEqualToAnchor: layoutGuide.centerXAnchor]];
            
            if ( [adViewPosition isEqual: @"top_center"] )
            {
                [constraints addObject: [adView.topAnchor constraintEqualToAnchor: layoutGuide.topAnchor]];
            }
            else // BottomCenter
            {
                [constraints addObject: [adView.bottomAnchor constraintEqualToAnchor: layoutGuide.bottomAnchor]];
            }
        }
    }
    // Check if the publisher wants vertical banners.
    else if ( [adViewPosition isEqual: @"center_left"] || [adViewPosition isEqual: @"center_right"] )
    {
        if ( MAAdFormat.mrec == adFormat )
        {
            [constraints addObject: [adView.widthAnchor constraintEqualToConstant: adViewSize.width]];
            
            if ( [adViewPosition isEqual: @"center_left"] )
            {
                [constraints addObjectsFromArray: @[[adView.centerYAnchor constraintEqualToAnchor: layoutGuide.centerYAnchor],
                                                    [adView.leftAnchor constraintEqualToAnchor: superview.leftAnchor]]];
                
                [constraints addObjectsFromArray: @[[self.safeAreaBackground.rightAnchor constraintEqualToAnchor: layoutGuide.leftAnchor],
                                                    [self.safeAreaBackground.leftAnchor constraintEqualToAnchor: superview.leftAnchor]]];
            }
            else // center_right
            {
                [constraints addObjectsFromArray: @[[adView.centerYAnchor constraintEqualToAnchor: layoutGuide.centerYAnchor],
                                                    [adView.rightAnchor constraintEqualToAnchor: superview.rightAnchor]]];
                
                [constraints addObjectsFromArray: @[[self.safeAreaBackground.leftAnchor constraintEqualToAnchor: layoutGuide.rightAnchor],
                                                    [self.safeAreaBackground.rightAnchor constraintEqualToAnchor: superview.rightAnchor]]];
            }
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
             *                  <+> |           |
             *                  |+  |           |
             *                  ||  |           |
             *                  ||  |           |
             *                  ||  |           |
             *                  ||  |           |
             *                  +|--+-----------+
             *                   v
             *            Banner Half Height
             */
            self.safeAreaBackground.hidden = YES;
            
            adView.transform = CGAffineTransformRotate(CGAffineTransformIdentity, M_PI_2);
            
            CGFloat width;
            // If the publisher has a background color set - set the width to the height of the screen, to span the ad across the screen after it is rotated.
            if ( self.publisherBannerBackgroundColor )
            {
                width = CGRectGetHeight(KEY_WINDOW.bounds);
            }
            // Otherwise - we shouldn't span the banner the width of the realm (there might be user-interactable UI on the sides)
            else
            {
                width = adViewSize.width;
            }
            [constraints addObject: [adView.widthAnchor constraintEqualToConstant: width]];
            
            // Set constraints such that the center of the banner aligns with the center left or right as needed. That way, once rotated, the banner snaps into place.
            [constraints addObject: [adView.centerYAnchor constraintEqualToAnchor: superview.centerYAnchor]];
            
            // Place the center of the banner half the height of the banner away from the side. If we align the center exactly with the left/right anchor, only half the banner will be visible.
            CGFloat bannerHalfHeight = adViewSize.height / 2.0;
            UIInterfaceOrientation orientation = [UIApplication sharedApplication].statusBarOrientation;
            if ( [adViewPosition isEqual: @"center_left"] )
            {
                NSLayoutAnchor *anchor = ( orientation == UIInterfaceOrientationLandscapeRight ) ? layoutGuide.leftAnchor : superview.leftAnchor;
                [constraints addObject: [adView.centerXAnchor constraintEqualToAnchor: anchor constant: bannerHalfHeight]];
            }
            else // CenterRight
            {
                NSLayoutAnchor *anchor = ( orientation == UIInterfaceOrientationLandscapeLeft ) ? layoutGuide.rightAnchor : superview.rightAnchor;
                [constraints addObject: [adView.centerXAnchor constraintEqualToAnchor: anchor constant: -bannerHalfHeight]];
            }
            
            // Store the ad view with format, so that it can be updated when the orientation changes.
            self.verticalAdViewFormats[adUnitIdentifier] = adFormat;
        }
    }
    // Otherwise, publisher will likely construct their own views around the adview
    else
    {
        self.safeAreaBackground.hidden = YES;
        
        // Assign constant width of 320 or 728
        [constraints addObject: [adView.widthAnchor constraintEqualToConstant: adViewSize.width]];
        
        if ( [adViewPosition isEqual: @"top_left"] )
        {
            [constraints addObjectsFromArray: @[[adView.topAnchor constraintEqualToAnchor: layoutGuide.topAnchor],
                                                [adView.leftAnchor constraintEqualToAnchor: superview.leftAnchor]]];
        }
        else if ( [adViewPosition isEqual: @"top_right"] )
        {
            [constraints addObjectsFromArray: @[[adView.topAnchor constraintEqualToAnchor: layoutGuide.topAnchor],
                                                [adView.rightAnchor constraintEqualToAnchor: superview.rightAnchor]]];
        }
        else if ( [adViewPosition isEqual: @"centered"] )
        {
            [constraints addObjectsFromArray: @[[adView.centerXAnchor constraintEqualToAnchor: layoutGuide.centerXAnchor],
                                                [adView.centerYAnchor constraintEqualToAnchor: layoutGuide.centerYAnchor]]];
        }
        else if ( [adViewPosition isEqual: @"bottom_left"] )
        {
            [constraints addObjectsFromArray: @[[adView.bottomAnchor constraintEqualToAnchor: layoutGuide.bottomAnchor],
                                                [adView.leftAnchor constraintEqualToAnchor: superview.leftAnchor]]];
        }
        else if ( [adViewPosition isEqual: @"bottom_right"] )
        {
            [constraints addObjectsFromArray: @[[adView.bottomAnchor constraintEqualToAnchor: layoutGuide.bottomAnchor],
                                                [adView.rightAnchor constraintEqualToAnchor: superview.rightAnchor]]];
        }
    }
    
    self.adViewConstraints[adUnitIdentifier] = constraints;
    
    [NSLayoutConstraint activateConstraints: constraints];
}

#pragma mark - Utility Methods

- (BOOL)isInvalidAdFormat:(MAAdFormat *)adFormat
{
    return !adFormat || !( [adFormat isAdViewAd] || MAAdFormat.interstitial == adFormat || MAAdFormat.rewarded == adFormat );
}

- (NSDictionary<NSString *, id> *)adInfoForAd:(MAAd *)ad
{
    return @{@"adUnitIdentifier" : ad.adUnitIdentifier,
             @"creativeIdentifier" : ad.creativeIdentifier ?: @"",
             @"networkName" : ad.networkName,
             @"placement" : ad.placement ?: @"",
             @"revenue" : @(ad.revenue)};
}

- (NSDictionary<NSString *, id> *)errorInfoForError:(MAError *)error
{
    return @{@"code" : @(error.code),
             @"message" : error.message ?: @"",
             @"waterfall" : error.waterfall.description ?: @""};
}

- (NSString *)eventNameForAdFormat:(MAAdFormat *)adFormat event:(NSString *)event
{
    if ( adFormat )
    {
        if ( [adFormat isAdViewAd] )
        {
            return [NSString stringWithFormat: @"OnBannerAd%@Event", event];
        }
        else if ( MAAdFormat.mrec == adFormat )
        {
            return [NSString stringWithFormat: @"OnMRecAd%@Event", event];
        }
        else if ( MAAdFormat.interstitial == adFormat )
        {
            return [NSString stringWithFormat: @"OnInterstitialAd%@Event", event];
        }
        else if ( MAAdFormat.rewarded == adFormat )
        {
            return [NSString stringWithFormat: @"OnRewardedAd%@Event", event];
        }
    }
    
    [NSException raise: NSInvalidArgumentException format: @"Invalid ad format"];
    return @"";
}

- (ALConsentFlowUserGeography)userGeographyForString:(NSString *)userGeographyString
{
    if ( [userGeographyString al_isEqualToStringIgnoringCase: @"UNKNOWN"] )
    {
        return ALConsentFlowUserGeographyUnknown;
    }
    else if ( [userGeographyString al_isEqualToStringIgnoringCase: @"GDPR"] )
    {
        return ALConsentFlowUserGeographyGDPR;
    }
    else
    {
        return ALConsentFlowUserGeographyOther;
    }
}

#pragma mark - Logging

- (void)d:(NSString *)format, ...
{
    va_list valist;
    va_start(valist, format);
    NSString *message = [[NSString alloc] initWithFormat: format arguments: valist];
    va_end(valist);
    
    NSLog(@"DEBUG [%@] [%@] %@", SDK_TAG, TAG, message);
}

- (void)e:(NSString *)format, ...
{
    va_list valist;
    va_start(valist, format);
    NSString *message = [[NSString alloc] initWithFormat: format arguments: valist];
    va_end(valist);
    
    NSLog(@"ERROR [%@] [%@] %@", SDK_TAG, TAG, message);
}

- (void)logInvalidAdFormat:(MAAdFormat *)adFormat
{
    NSString *message = [NSString stringWithFormat: @"Invalid ad format: %@", adFormat];
    [self logStackTrace: message];
}

- (void)logInvalidAdUnitIdentifier:(NSString *)adUnitIdentifier forAdFormat:(MAAdFormat *)adFormat
{
    [self e: @"%@ does not exist for %@", adFormat.label, adUnitIdentifier];
}                                                                  

- (void)logStackTrace:(NSString *)message
{
    [self e: message];
    [self e: @"%@", [NSThread callStackSymbols]];
}

- (void)logUninitializedError:(NSString *)message
{
    [self e: @"%@ - Please ensure the AppLovin MAX Unreal Plugin has been initialized by calling UAppLovinMAX::Initialize()", message];
}

#pragma mark - Unreal Bridge

// NOTE: Unreal deserializes to the relevant USTRUCT based on the JSON keys, so the keys must match with the corresponding UPROPERTY
- (void)sendUnrealEventWithName:(NSString *)name parameters:(NSDictionary<NSString *, id> *)parameters
{
    if ( !self.eventCallback ) return;
    
    NSData *data = [NSJSONSerialization dataWithJSONObject: parameters options: 0 error: nil];
    NSString *serializedParameters = [[NSString alloc] initWithData: data encoding: NSUTF8StringEncoding];
    
    self.eventCallback(name, serializedParameters);
}

@end
