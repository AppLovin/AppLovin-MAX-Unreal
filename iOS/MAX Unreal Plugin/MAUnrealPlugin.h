//
//  MAUnrealPlugin.h
//  MAX Unreal Plugin
//
//  Created by Ritam Sarmah on 5/9/21.
//  Copyright © 2021 AppLovin. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AppLovinSDK/AppLovinSDK.h>

NS_ASSUME_NONNULL_BEGIN

@interface MAUnrealPlugin : NSObject

typedef void(*UnrealEventCallback)(NSString *name, NSString *body);

#pragma mark - Initialization

- (instancetype)initWithView:(UIView *)mainView eventCallback:(UnrealEventCallback)eventCallback;
- (void)initialize:(NSString *)pluginVersion sdkKey:(NSString *)sdkKey;
- (BOOL)isInitialized;

#pragma mark - Privacy

- (void)setHasUserConsent:(BOOL)hasUserConsent;
- (BOOL)hasUserConsent;
- (void)setIsAgeRestrictedUser:(BOOL)isAgeRestrictedUser;
- (BOOL)isAgeRestrictedUser;
- (void)setDoNotSell:(BOOL)doNotSell;
- (BOOL)isDoNotSell;

#pragma mark - Terms and Privacy Policy Flow

- (void)setTermsAndPrivacyPolicyFlowEnabled:(BOOL)enabled;
- (void)setPrivacyPolicyURL:(NSString *)urlString;
- (void)setTermsOfServiceURL:(NSString *)urlString;

#pragma mark - General

- (BOOL)isTablet;
- (void)showMediationDebugger;
- (void)setUserId:(NSString *)userId;
- (void)setMuted:(BOOL)muted;
- (BOOL)isMuted;
- (void)setVerboseLoggingEnabled:(BOOL)enabled;
- (BOOL)isVerboseLoggingEnabled;
- (void)setCreativeDebuggerEnabled:(BOOL)enabled;
- (void)setTestDeviceAdvertisingIds:(NSArray<NSString *> *)testDeviceAdvertisingIds;

#pragma mark - Event Tracking

- (void)trackEvent:(NSString *)event parameters:(NSDictionary<NSString *, NSString *> *)parameters;

#pragma mark - Banners

- (void)createBannerWithAdUnitIdentifier:(NSString *)adUnitIdentifier atPosition:(NSString *)bannerPosition;
- (void)setBannerBackgroundColorForAdUnitIdentifier:(NSString *)adUnitIdentifier hexColorCode:(NSString *)hexColorCode;
- (void)setBannerPlacement:(nullable NSString *)placement forAdUnitIdentifier:(NSString *)adUnitIdentifier;
- (void)setBannerExtraParameterForAdUnitIdentifier:(NSString *)adUnitIdentifier key:(NSString *)key value:(nullable NSString *)value;
- (void)updateBannerPosition:(NSString *)bannerPosition forAdUnitIdentifier:(NSString *)adUnitIdentifier;
- (void)showBannerWithAdUnitIdentifier:(NSString *)adUnitIdentifier;
- (void)hideBannerWithAdUnitIdentifier:(NSString *)adUnitIdentifier;
- (void)destroyBannerWithAdUnitIdentifier:(NSString *)adUnitIdentifier;

#pragma mark - MRECs

- (void)createMRecWithAdUnitIdentifier:(NSString *)adUnitIdentifier atPosition:(NSString *)mrecPosition;
- (void)setMRecPlacement:(nullable NSString *)placement forAdUnitIdentifier:(NSString *)adUnitIdentifier;
- (void)setMRecExtraParameterForAdUnitIdentifier:(NSString *)adUnitIdentifier key:(NSString *)key value:(nullable NSString *)value;
- (void)updateMRecPosition:(NSString *)mrecPosition forAdUnitIdentifier:(NSString *)adUnitIdentifier;
- (void)showMRecWithAdUnitIdentifier:(NSString *)adUnitIdentifier;
- (void)hideMRecWithAdUnitIdentifier:(NSString *)adUnitIdentifier;
- (void)destroyMRecWithAdUnitIdentifier:(NSString *)adUnitIdentifier;

#pragma mark - Interstitials

- (void)loadInterstitialWithAdUnitIdentifier:(NSString *)adUnitIdentifier;
- (BOOL)isInterstitialReadyWithAdUnitIdentifier:(NSString *)adUnitIdentifier;
- (void)showInterstitialWithAdUnitIdentifier:(NSString *)adUnitIdentifier placement:(NSString *)placement;
- (void)setInterstitialExtraParameterForAdUnitIdentifier:(NSString *)adUnitIdentifier key:(NSString *)key value:(NSString *)value;

#pragma mark - Rewarded

- (void)loadRewardedAdWithAdUnitIdentifier:(NSString *)adUnitIdentifier;
- (BOOL)isRewardedAdReadyWithAdUnitIdentifier:(NSString *)adUnitIdentifier;
- (void)showRewardedAdWithAdUnitIdentifier:(NSString *)adUnitIdentifier placement:(NSString *)placement;
- (void)setRewardedAdExtraParameterForAdUnitIdentifier:(NSString *)adUnitIdentifier key:(NSString *)key value:(nullable NSString *)value;

@end

NS_ASSUME_NONNULL_END
