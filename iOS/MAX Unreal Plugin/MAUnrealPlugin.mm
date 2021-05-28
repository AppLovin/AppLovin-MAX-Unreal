//
//  MAUnrealPlugin.m
//  MAX Unreal Plugin
//
//  Created by Ritam Sarmah on 5/9/21.
//  Copyright © 2021 AppLovin. All rights reserved.
//

#import "MAUnrealPlugin.h"

#define KEY_WINDOW [UIApplication sharedApplication].keyWindow
#define DEVICE_SPECIFIC_ADVIEW_AD_FORMAT ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPad) ? MAAdFormat.leader : MAAdFormat.banner

#ifdef __cplusplus
extern "C" {
#endif
void dispatchOnMainQueue(dispatch_block_t block)
{
    if ( block )
    {
        if ( [NSThread isMainThread] )
        {
            block();
        }
        else
        {
            dispatch_async(dispatch_get_main_queue(), block);
        }
    }
}
#ifdef __cplusplus
}
#endif

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

@interface MAUnrealPlugin()<MAAdDelegate, MAAdViewAdDelegate, MARewardedAdDelegate>

// Parent Fields
@property (nonatomic, weak) ALSdk *sdk;
@property (nonatomic, assign, getter=isPluginInitialized) BOOL pluginInitialized;
@property (nonatomic, assign, getter=isSDKInitialized) BOOL sdkInitialized;
@property (nonatomic, strong) ALSdkConfiguration *sdkConfiguration;

// Store these values if pub attempts to set it before initializing
@property (nonatomic,   copy, nullable) NSString *userIdentifierToSet;
@property (nonatomic, strong, nullable) NSArray<NSString *> *testDeviceIdentifiersToSet;
@property (nonatomic, strong, nullable) NSNumber *verboseLoggingToSet;
@property (nonatomic, strong, nullable) NSNumber *creativeDebuggerEnabledToSet;

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

@property (nonatomic, weak) UIView *unrealMainView;
@property (nonatomic, assign) UnrealEventCallback eventCallback;

@end

@implementation MAUnrealPlugin
static NSString *const SDK_TAG = @"AppLovinSdk";
static NSString *const TAG = @"MAUnrealPlugin";

static NSString *const ALSerializeKeyValueSeparator = [NSString stringWithFormat: @"%c", 28];
static NSString *const ALSerializeKeyValuePairSeparator = [NSString stringWithFormat: @"%c", 29];

#pragma mark - Initialization

- (instancetype)initWithView:(UIView *)mainView eventCallback:(UnrealEventCallback)eventCallback
{
    self = [super init];
    if ( self )
    {
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
        
        dispatchOnMainQueue(^{
            self.safeAreaBackground = [[UIView alloc] init];
            self.safeAreaBackground.hidden = YES;
            self.safeAreaBackground.backgroundColor = UIColor.clearColor;
            self.safeAreaBackground.translatesAutoresizingMaskIntoConstraints = NO;
            self.safeAreaBackground.userInteractionEnabled = NO;
            
            [self.unrealMainView addSubview: self.safeAreaBackground];
        });
        
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
    }
    return self;
}

- (void)initialize:(NSString *)pluginVersion sdkKey:(NSString *)sdkKey
{
    // TODO: Temporary Swizzling
    NSDictionary *infoDict = [[NSBundle mainBundle] infoDictionary];
    [infoDict setValue: @"com.revolverolver.flipmania" forKey: @"CFBundleIdentifier"];
    
    // Guard against running init logic multiple times
    if ( [self isPluginInitialized] )
    {
        [self sendUnrealEventWithName: @"OnSdkInitializedEvent" parameters: [self initializationMessage]];
        return;
    }
    
    self.pluginInitialized = YES;
    
    [self log: @"Initializing AppLovin MAX Unreal v%@...", pluginVersion];
    
    // If SDK key passed in is empty, check Info.plist
    if ( ![sdkKey al_isValidString] )
    {
        if ( [[NSBundle mainBundle].infoDictionary al_containsValueForKey: @"AppLovinSdkKey"] )
        {
            sdkKey = [NSBundle mainBundle].infoDictionary[@"AppLovinSdkKey"];
        }
        else
        {
            [NSException raise: NSInternalInconsistencyException
                        format: @"Unable to initialize AppLovin SDK - no SDK key provided and not found in Info.plist!"];
        }
    }
    
    // Initialize SDK
    self.sdk = [ALSdk sharedWithKey: sdkKey];
    [self.sdk setPluginVersion: [@"Unreal-" stringByAppendingString: pluginVersion]];
    [self.sdk setMediationProvider: ALMediationProviderMAX];
    
    // Set user id if needed
    if ( [self.userIdentifierToSet al_isValidString] )
    {
        self.sdk.userIdentifier = self.userIdentifierToSet;
        self.userIdentifierToSet = nil;
    }
    
    // Set test device ids if needed
    if ( self.testDeviceIdentifiersToSet )
    {
        self.sdk.settings.testDeviceAdvertisingIdentifiers = self.testDeviceIdentifiersToSet;
        self.testDeviceIdentifiersToSet = nil;
    }
    
    // Set verbose logging state if needed
    if ( self.verboseLoggingToSet )
    {
        self.sdk.settings.isVerboseLogging = self.verboseLoggingToSet.boolValue;
        self.verboseLoggingToSet = nil;
    }
    
    // Set creative debugger enabled if needed
    if ( self.creativeDebuggerEnabledToSet )
    {
        self.sdk.settings.creativeDebuggerEnabled = self.creativeDebuggerEnabledToSet.boolValue;
        self.creativeDebuggerEnabledToSet = nil;
    }
    
    [self.sdk initializeSdkWithCompletionHandler:^(ALSdkConfiguration *configuration) {
        [self log: @"SDK initialized"];
        
        self.sdkConfiguration = configuration;
        self.sdkInitialized = YES;
        
        [self sendUnrealEventWithName: @"OnSdkInitializedEvent" parameters: [self initializationMessage]];
    }];
}

- (NSDictionary<NSString *, id> *)initializationMessage
{
    return @{@"consentDialogState" : [@(self.sdkConfiguration.consentDialogState) stringValue],
             @"appTrackingStatus" : [@(self.sdkConfiguration.appTrackingTransparencyStatus) stringValue],
             @"countryCode" : self.sdkConfiguration.countryCode};
}

- (BOOL)isInitialized
{
    return [self isPluginInitialized] && [self isSDKInitialized];
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

- (void)setIsAgeRestrictedUser:(BOOL)isAgeRestrictedUser
{
    [ALPrivacySettings setIsAgeRestrictedUser: isAgeRestrictedUser];
}

- (BOOL)isAgeRestrictedUser
{
    return [ALPrivacySettings isAgeRestrictedUser];
}

- (void)setDoNotSell:(BOOL)doNotSell
{
    [ALPrivacySettings setDoNotSell: doNotSell];
}

- (BOOL)isDoNotSell
{
    return [ALPrivacySettings isDoNotSell];
}

#pragma mark - General

- (BOOL)isTablet
{
    return [UIDevice currentDevice].userInterfaceIdiom == UIUserInterfaceIdiomPad;
}

- (void)showMediationDebugger
{
    if ( !_sdk )
    {
        [self log: @"Failed to show mediation debugger - please ensure the AppLovin MAX Unreal Plugin has been initialized by calling 'UAppLovinMAX::Initialize(...);'!"];
        return;
    }
    
    [self.sdk showMediationDebugger];
}

- (void)setUserId:(NSString *)userId
{
    if ( [self isPluginInitialized] )
    {
        self.sdk.userIdentifier = userId;
        self.userIdentifierToSet = nil;
    }
    else
    {
        self.userIdentifierToSet = userId;
    }
}

- (void)setMuted:(BOOL)muted
{
    if ( ![self isPluginInitialized] ) return;
    
    self.sdk.settings.muted = muted;
}

- (BOOL)isMuted
{
    if ( ![self isPluginInitialized] ) return false;
    
    return self.sdk.settings.muted;
}

- (void)setVerboseLoggingEnabled:(BOOL)enabled
{
    if ( [self isPluginInitialized] )
    {
        self.sdk.settings.isVerboseLogging = enabled;
        self.verboseLoggingToSet = nil;
    }
    else
    {
        self.verboseLoggingToSet = @(enabled);
    }
}

- (BOOL)isVerboseLoggingEnabled
{
    if ( self.sdk )
    {
        return self.sdk.settings.isVerboseLogging;
    }
    else if ( self.verboseLoggingToSet )
    {
        return self.verboseLoggingToSet.boolValue;
    }
    
    return false;
}

- (void)setCreativeDebuggerEnabled:(BOOL)enabled
{
    if ( [self isPluginInitialized] )
    {
        self.sdk.settings.creativeDebuggerEnabled = enabled;
        self.creativeDebuggerEnabledToSet = nil;
    }
    else
    {
        self.creativeDebuggerEnabledToSet = @(enabled);
    }
}

- (void)setTestDeviceAdvertisingIds:(NSArray<NSString *> *)testDeviceAdvertisingIds
{
    if ( [self isPluginInitialized] )
    {
        self.sdk.settings.testDeviceAdvertisingIdentifiers = testDeviceAdvertisingIds;
        self.testDeviceIdentifiersToSet = nil;
    }
    else
    {
        self.testDeviceIdentifiersToSet = testDeviceAdvertisingIds;
    }
}

#pragma mark - Event Tracking

- (void)trackEvent:(NSString *)event parameters:(NSDictionary<NSString *, NSString *> *)parameters
{
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
    NSString *name;
    MAAdFormat *adFormat = ad.format;
    if ( MAAdFormat.banner == adFormat || MAAdFormat.leader == adFormat || MAAdFormat.mrec == adFormat )
    {
        MAAdView *adView = [self retrieveAdViewForAdUnitIdentifier: ad.adUnitIdentifier adFormat: adFormat];
        // An ad is now being shown, enable user interaction.
        adView.userInteractionEnabled = YES;
        
        name = ( MAAdFormat.mrec == adFormat ) ? @"OnMRecAdLoadedEvent" : @"OnBannerAdLoadedEvent";
        [self positionAdViewForAd: ad];
        
        // Do not auto-refresh by default if the ad view is not showing yet (e.g. first load during app launch and publisher does not automatically show banner upon load success)
        // We will resume auto-refresh in -[MAUnrealPlugin showBannerWithAdUnitIdentifier:].
        if ( adView && [adView isHidden] )
        {
            [adView stopAutoRefresh];
        }
    }
    else if ( MAAdFormat.interstitial == adFormat )
    {
        name = @"OnInterstitialLoadedEvent";
    }
    else if ( MAAdFormat.rewarded == adFormat )
    {
        name = @"OnRewardedAdLoadedEvent";
    }
    else
    {
        [self logInvalidAdFormat: adFormat];
        return;
    }
    
    [self sendUnrealEventWithName: name parameters: [self adInfoForAd: ad]];
}

- (void)didFailToLoadAdForAdUnitIdentifier:(NSString *)adUnitIdentifier withError:(nonnull MAError *)error
{
    if ( !adUnitIdentifier )
    {
        [self log: @"adUnitIdentifier cannot be nil from %@", [NSThread callStackSymbols]];
        return;
    }
    
    NSString *name;
    if ( self.adViews[adUnitIdentifier] )
    {
        name = ( MAAdFormat.mrec == self.adViewAdFormats[adUnitIdentifier] ) ? @"OnMRecAdLoadFailedEvent" : @"OnBannerAdLoadFailedEvent";
    }
    else if ( self.interstitials[adUnitIdentifier] )
    {
        name = @"OnInterstitialLoadFailedEvent";
    }
    else if ( self.rewardedAds[adUnitIdentifier] )
    {
        name = @"OnRewardedAdLoadFailedEvent";
    }
    else
    {
        [self log: @"invalid adUnitId from %@", [NSThread callStackSymbols]];
        return;
    }
    
    NSMutableDictionary *parameters = [[self errorInfoForError: error] mutableCopy];
    parameters[@"adUnitId"] = adUnitIdentifier;
    
    [self sendUnrealEventWithName: name parameters: parameters];
}

- (void)didClickAd:(MAAd *)ad
{
    NSString *name;
    MAAdFormat *adFormat = ad.format;
    if ( MAAdFormat.banner == adFormat || MAAdFormat.leader == adFormat )
    {
        name = @"OnBannerAdClickedEvent";
    }
    else if ( MAAdFormat.mrec == adFormat )
    {
        name = @"OnMRecAdClickedEvent";
    }
    else if ( MAAdFormat.interstitial == adFormat )
    {
        name = @"OnInterstitialClickedEvent";
    }
    else if ( MAAdFormat.rewarded == adFormat )
    {
        name = @"OnRewardedAdClickedEvent";
    }
    else
    {
        [self logInvalidAdFormat: adFormat];
        return;
    }
    
    [self sendUnrealEventWithName: name parameters: [self adInfoForAd: ad]];
}

- (void)didDisplayAd:(MAAd *)ad
{
    // BMLs do not support [DISPLAY] events
    MAAdFormat *adFormat = ad.format;
    if ( adFormat != MAAdFormat.interstitial && adFormat != MAAdFormat.rewarded ) return;
    
    NSString *name;
    if ( MAAdFormat.interstitial == adFormat )
    {
        name = @"OnInterstitialDisplayedEvent";
    }
    else // REWARDED
    {
        name = @"OnRewardedAdDisplayedEvent";
    }
    
    [self sendUnrealEventWithName: name parameters: [self adInfoForAd: ad]];
}

- (void)didFailToDisplayAd:(MAAd *)ad withError:(MAError *)error
{
    // BMLs do not support [DISPLAY] events
    MAAdFormat *adFormat = ad.format;
    if ( adFormat != MAAdFormat.interstitial && adFormat != MAAdFormat.rewarded ) return;
    
    NSString *name;
    if ( MAAdFormat.interstitial == adFormat )
    {
        name = @"OnInterstitialAdFailedToDisplayEvent";
    }
    else // REWARDED
    {
        name = @"OnRewardedAdFailedToDisplayEvent";
    }
    
    NSMutableDictionary *parameters = [[self adInfoForAd: ad] mutableCopy];
    [parameters addEntriesFromDictionary: [self errorInfoForError: error]];
    
    [self sendUnrealEventWithName: name parameters: parameters];
}

- (void)didHideAd:(MAAd *)ad
{
    // BMLs do not support [HIDDEN] events
    MAAdFormat *adFormat = ad.format;
    if ( adFormat != MAAdFormat.interstitial && adFormat != MAAdFormat.rewarded ) return;
    
    NSString *name;
    if ( MAAdFormat.interstitial == adFormat )
    {
        name = @"OnInterstitialHiddenEvent";
    }
    else // REWARDED
    {
        name = @"OnRewardedAdHiddenEvent";
    }
    
    [self sendUnrealEventWithName: name parameters: [self adInfoForAd: ad]];
}

- (void)didExpandAd:(MAAd *)ad
{
    MAAdFormat *adFormat = ad.format;
    if ( adFormat != MAAdFormat.banner && adFormat != MAAdFormat.leader && adFormat != MAAdFormat.mrec )
    {
        [self logInvalidAdFormat: adFormat];
        return;
    }
    
    [self sendUnrealEventWithName: ( MAAdFormat.mrec == adFormat ) ? @"OnMRecAdExpandedEvent" : @"OnBannerAdExpandedEvent"
                       parameters: [self adInfoForAd: ad]];
}

- (void)didCollapseAd:(MAAd *)ad
{
    MAAdFormat *adFormat = ad.format;
    if ( adFormat != MAAdFormat.banner && adFormat != MAAdFormat.leader && adFormat != MAAdFormat.mrec )
    {
        [self logInvalidAdFormat: adFormat];
        return;
    }
    
    [self sendUnrealEventWithName: ( MAAdFormat.mrec == adFormat ) ? @"OnMRecAdCollapsedEvent" : @"OnBannerAdCollapsedEvent"
                       parameters: [self adInfoForAd: ad]];
}

- (void)didCompleteRewardedVideoForAd:(MAAd *)ad
{
    // This event is not forwarded
}

- (void)didStartRewardedVideoForAd:(MAAd *)ad
{
    // This event is not forwarded
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
    parameters[@"rewardLabel"] = reward ? reward.label : @"";
    NSInteger rewardAmountInt = reward ? reward.amount : 0;
    parameters[@"rewardAmount"] = [@(rewardAmountInt) stringValue];
    
    [self sendUnrealEventWithName: @"OnRewardedAdReceivedRewardEvent" parameters: parameters];
}

- (void)didPayRevenueForAd:(MAAd *)ad
{
    NSString *name;
    MAAdFormat *adFormat = ad.format;
    if ( MAAdFormat.banner == adFormat || MAAdFormat.leader == adFormat )
    {
        name = @"OnBannerAdRevenuePaidEvent";
    }
    else if ( MAAdFormat.mrec == adFormat )
    {
        name = @"OnMRecAdRevenuePaidEvent";
    }
    else if ( MAAdFormat.interstitial == adFormat )
    {
        name = @"OnInterstitialAdRevenuePaidEvent";
    }
    else if ( MAAdFormat.rewarded == adFormat )
    {
        name = @"OnRewardedAdRevenuePaidEvent";
    }
    else
    {
        [self logInvalidAdFormat: adFormat];
        return;
    }
    
    [self sendUnrealEventWithName: name parameters: [self adInfoForAd: ad]];
}

#pragma mark - Internal Methods

- (void)createAdViewWithAdUnitIdentifier:(NSString *)adUnitIdentifier adFormat:(MAAdFormat *)adFormat atPosition:(NSString *)adViewPosition
{
    dispatchOnMainQueue(^{
        [self log: @"Creating %@ with ad unit identifier \"%@\" and position: \"%@\"", adFormat, adUnitIdentifier, adViewPosition];
        
        // Retrieve ad view from the map
        MAAdView *adView = [self retrieveAdViewForAdUnitIdentifier: adUnitIdentifier adFormat: adFormat atPosition: adViewPosition];
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
        [self log: @"Setting %@ with ad unit identifier \"%@\" to color: \"%@\"", adFormat, adUnitIdentifier, hexColorCode];
        
        // In some cases, black color may get redrawn on each frame update, resulting in an undesired flicker
        NSString *hexColorCodeToUse = [hexColorCode containsString: @"FF000000"] ? @"FF000001" : hexColorCode;
        UIColor *convertedColor = [UIColor al_colorWithHexString: hexColorCodeToUse];
        
        MAAdView *view = [self retrieveAdViewForAdUnitIdentifier: adUnitIdentifier adFormat: adFormat];
        self.publisherBannerBackgroundColor = convertedColor;
        self.safeAreaBackground.backgroundColor = view.backgroundColor = convertedColor;
    });
}

- (void)setAdViewPlacement:(nullable NSString *)placement forAdUnitIdentifier:(NSString *)adUnitIdentifier adFormat:(MAAdFormat *)adFormat
{
    dispatchOnMainQueue(^{
        [self log: @"Setting placement \"%@\" for \"%@\" with ad unit identifier \"%@\"", placement, adFormat, adUnitIdentifier];
        
        MAAdView *adView = [self retrieveAdViewForAdUnitIdentifier: adUnitIdentifier adFormat: adFormat];
        adView.placement = placement;
    });
}

- (void)updateAdViewPosition:(NSString *)adViewPosition forAdUnitIdentifier:(NSString *)adUnitIdentifier adFormat:(MAAdFormat *)adFormat
{
    dispatchOnMainQueue(^{
        // Check if the previous position is same as the new position. If so, no need to update the position again.
        NSString *previousPosition = self.adViewPositions[adUnitIdentifier];
        if ( !adViewPosition || [adViewPosition isEqualToString: previousPosition] ) return;
        
        self.adViewPositions[adUnitIdentifier] = adViewPosition;
        [self positionAdViewForAdUnitIdentifier: adUnitIdentifier adFormat: adFormat];
    });
}

- (void)setAdViewExtraParameterForAdUnitIdentifier:(NSString *)adUnitIdentifier adFormat:(MAAdFormat *)adFormat key:(NSString *)key value:(nullable NSString *)value
{
    dispatchOnMainQueue(^{
        [self log: @"Setting %@ extra with key: \"%@\" value: \"%@\"", adFormat, key, value];
        
        MAAdView *adView = [self retrieveAdViewForAdUnitIdentifier: adUnitIdentifier adFormat: adFormat];
        [adView setExtraParameterForKey: key value: value];
        
        if (  [@"force_banner" isEqualToString: key] && MAAdFormat.mrec != adFormat )
        {
            // Handle local changes as needed
            MAAdFormat *forcedAdFormat;
            
            BOOL shouldForceBanner = [NSNumber al_numberWithString: value].boolValue;
            if ( shouldForceBanner )
            {
                forcedAdFormat = MAAdFormat.banner;
            }
            else
            {
                forcedAdFormat = DEVICE_SPECIFIC_ADVIEW_AD_FORMAT;
            }
            
            self.adViewAdFormats[adUnitIdentifier] = forcedAdFormat;
            [self positionAdViewForAdUnitIdentifier: adUnitIdentifier adFormat: forcedAdFormat];
        }
    });
}

- (void)showAdViewWithAdUnitIdentifier:(NSString *)adUnitIdentifier adFormat:(MAAdFormat *)adFormat
{
    dispatchOnMainQueue(^{
        [self log: @"Showing %@ with ad unit identifier \"%@\"", adFormat, adUnitIdentifier];
        
        MAAdView *view = [self retrieveAdViewForAdUnitIdentifier: adUnitIdentifier adFormat: adFormat];
        if ( !view )
        {
            [self log: @"%@ does not exist for ad unit identifier %@.", adFormat, adUnitIdentifier];
            
            // The adView has not yet been created. Store the ad unit ID, so that it can be displayed once the banner has been created.
            [self.adUnitIdentifiersToShowAfterCreate addObject: adUnitIdentifier];
        }
        
        self.safeAreaBackground.hidden = NO;
        view.hidden = NO;
        
        [view startAutoRefresh];
    });
}

- (void)hideAdViewWithAdUnitIdentifier:(NSString *)adUnitIdentifier adFormat:(MAAdFormat *)adFormat
{
    dispatchOnMainQueue(^{
        [self log: @"Hiding %@ with ad unit identifier \"%@\"", adFormat, adUnitIdentifier];
        [self.adUnitIdentifiersToShowAfterCreate removeObject: adUnitIdentifier];
        
        MAAdView *view = [self retrieveAdViewForAdUnitIdentifier: adUnitIdentifier adFormat: adFormat];
        view.hidden = YES;
        self.safeAreaBackground.hidden = YES;
        
        [view stopAutoRefresh];
    });
}

- (void)destroyAdViewWithAdUnitIdentifier:(NSString *)adUnitIdentifier adFormat:(MAAdFormat *)adFormat
{
    dispatchOnMainQueue(^{
        [self log: @"Destroying %@ with ad unit identifier \"%@\"", adFormat, adUnitIdentifier];
        
        MAAdView *view = [self retrieveAdViewForAdUnitIdentifier: adUnitIdentifier adFormat: adFormat];
        view.delegate = nil;
        
        [view removeFromSuperview];
        
        [self.adViews removeObjectForKey: adUnitIdentifier];
        [self.adViewPositions removeObjectForKey: adUnitIdentifier];
        [self.adViewAdFormats removeObjectForKey: adUnitIdentifier];
        [self.verticalAdViewFormats removeObjectForKey: adUnitIdentifier];
    });
}

- (void)logInvalidAdFormat:(MAAdFormat *)adFormat
{
    [self log: @"invalid ad format: %@, from %@", adFormat, [NSThread callStackSymbols]];
}

- (void)log:(NSString *)format, ...
{
    va_list valist;
    va_start(valist, format);
    NSString *message = [[NSString alloc] initWithFormat: format arguments: valist];
    va_end(valist);
    
    NSLog(@"[%@] [%@] %@", SDK_TAG, TAG, message);
}

- (MAInterstitialAd *)retrieveInterstitialForAdUnitIdentifier:(NSString *)adUnitIdentifier
{
    MAInterstitialAd *result = self.interstitials[adUnitIdentifier];
    if ( !result )
    {
        result = [[MAInterstitialAd alloc] initWithAdUnitIdentifier: adUnitIdentifier sdk: self.sdk];
        result.delegate = self;
        
        self.interstitials[adUnitIdentifier] = result;
    }
    
    return result;
}

- (MARewardedAd *)retrieveRewardedAdForAdUnitIdentifier:(NSString *)adUnitIdentifier
{
    MARewardedAd *result = self.rewardedAds[adUnitIdentifier];
    if ( !result )
    {
        result = [MARewardedAd sharedWithAdUnitIdentifier: adUnitIdentifier sdk: self.sdk];
        result.delegate = self;
        
        self.rewardedAds[adUnitIdentifier] = result;
    }
    
    return result;
}

- (MAAdView *)retrieveAdViewForAdUnitIdentifier:(NSString *)adUnitIdentifier adFormat:(MAAdFormat *)adFormat
{
    return [self retrieveAdViewForAdUnitIdentifier: adUnitIdentifier adFormat: adFormat atPosition: nil];
}

- (MAAdView *)retrieveAdViewForAdUnitIdentifier:(NSString *)adUnitIdentifier adFormat:(MAAdFormat *)adFormat atPosition:(NSString *)adViewPosition
{
    MAAdView *result = self.adViews[adUnitIdentifier];
    if ( !result && adViewPosition )
    {
        result = [[MAAdView alloc] initWithAdUnitIdentifier: adUnitIdentifier adFormat: adFormat sdk: self.sdk];
        result.delegate = self;
        result.userInteractionEnabled = NO;
        result.translatesAutoresizingMaskIntoConstraints = NO;
        
        self.adViews[adUnitIdentifier] = result;
        
        self.adViewPositions[adUnitIdentifier] = adViewPosition;
        [self.unrealMainView addSubview: result];
    }
    
    return result;
}

- (void)positionAdViewForAd:(MAAd *)ad
{
    [self positionAdViewForAdUnitIdentifier: ad.adUnitIdentifier adFormat: ad.format];
}

- (void)positionAdViewForAdUnitIdentifier:(NSString *)adUnitIdentifier adFormat:(MAAdFormat *)adFormat
{
    MAAdView *adView = [self retrieveAdViewForAdUnitIdentifier: adUnitIdentifier adFormat: adFormat];
    NSString *adViewPosition = self.adViewPositions[adUnitIdentifier];
    
    UIView *superview = adView.superview;
    if ( !superview ) return;
    
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
    
    CGSize adViewSize = [[self class] adViewSizeForAdFormat: adFormat];
    
    // All positions have constant height
    NSMutableArray<NSLayoutConstraint *> *constraints = [NSMutableArray arrayWithObject: [adView.heightAnchor constraintEqualToConstant: adViewSize.height]];
    
    UILayoutGuide *layoutGuide;
    if ( @available(iOS 11.0, *) )
    {
        layoutGuide = superview.safeAreaLayoutGuide;
    }
    else
    {
        layoutGuide = superview.layoutMarginsGuide;
    }
    
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

+ (CGSize)adViewSizeForAdFormat:(MAAdFormat *)adFormat
{
    if ( MAAdFormat.leader == adFormat )
    {
        return CGSizeMake(728.0f, 90.0f);
    }
    else if ( MAAdFormat.banner == adFormat )
    {
        return CGSizeMake(320.0f, 50.0f);
    }
    else if ( MAAdFormat.mrec == adFormat )
    {
        return CGSizeMake(300.0f, 250.0f);
    }
    else
    {
        [NSException raise: NSInvalidArgumentException format: @"Invalid ad format"];
        return CGSizeZero;
    }
}

#pragma mark - Utility Methods

- (NSDictionary<NSString *, NSString *> *)adInfoForAd:(MAAd *)ad
{
    return @{@"adUnitId" : ad.adUnitIdentifier,
             @"creativeId" : ad.creativeIdentifier ?: @"",
             @"networkName" : ad.networkName,
             @"placement" : ad.placement ?: @"",
             @"revenue" : [@(ad.revenue) stringValue]};
}

- (NSDictionary<NSString *, NSString *> *)errorInfoForError:(MAError *)error
{
    return @{@"errorCode" : [@(error.code) stringValue],
             @"errorMessage" : error.message ?: @"",
             @"errorAdLoadFailureInfo" : error.adLoadFailureInfo ?: @""};
}

- (NSString *)serialize:(NSDictionary<NSString *, NSString *> *)dict
{
    NSMutableString *result = [[NSMutableString alloc] initWithCapacity: 64];
    [dict enumerateKeysAndObjectsUsingBlock:^(NSString *key, NSString *obj, BOOL *stop)
     {
        [result appendString: key];
        [result appendString: ALSerializeKeyValueSeparator];
        [result appendString: obj];
        [result appendString: ALSerializeKeyValuePairSeparator];
    }];
    
    return result;
}

#pragma mark - Unreal Bridge

- (void)sendUnrealEventWithName:(NSString *)name parameters:(NSDictionary<NSString *, NSString *> *)parameters
{
    NSString *serializedParameters = [self serialize: parameters];
    if ( self.eventCallback )
    {
        self.eventCallback(name, serializedParameters);
    }
}

@end
