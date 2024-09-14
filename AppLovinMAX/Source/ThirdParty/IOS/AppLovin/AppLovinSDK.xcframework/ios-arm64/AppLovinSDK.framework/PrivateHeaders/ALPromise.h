//
//  ALPromise.h
//  AppLovinSDK
//
//  Created by Paul Hounshell on 5/1/24.
//

NS_ASSUME_NONNULL_BEGIN

/**
 * Class that represents a promise to eventually have a value or fail.
 *
 * Once the class is created you may call setSuccess() or setFailure(). Subsequent calls to these methods will be
 * ignored. Once one of these has been called the Promise is considered "completed".
 *
 * You can ask the class if it has a value, and add listeners for when the success or failure value is set.
 * Once a listener is called, any references to it will be dropped. If the Promise has already completed then
 * the listener will be called immediately.
 *
 * This class is thread-safe, and can be used by multiple threads at the same time.
 */
@interface ALPromise <SuccessType, FailureType> : NSObject

typedef void (^PromiseCompleteCallback) (BOOL isSuccessful, _Nullable SuccessType successValue, _Nullable FailureType failureValue);
typedef void (^PromiseSuccessCallback) (SuccessType successValue);
typedef void (^PromiseFailureCallback) (FailureType failureValue);

#pragma mark - Properties

/**
 * Gets the name of this Promise, provided at creation time.
 */
@property (nonatomic, copy, readonly) NSString *name;

/**
 * Indicates whether the Promise has completed.
 */
@property (nonatomic, assign, readonly, getter=isComplete) BOOL complete;

/**
 * Indicates whether the Promise has completed successfully.
 */
@property (nonatomic, assign, readonly, getter=isSuccessful) BOOL successful;

/**
 * Indicates whether the Promise has completed with a failure.
 */
@property (nonatomic, assign, readonly, getter=isFailure) BOOL failure;

/**
 * Gets or sets a successful value. Only the first success or failure value is accepted, subsequent calls are ignored. The getter must not be called unless the Promise has completed successfully.
 */
@property (nonatomic, strong) SuccessType successValue;

/**
 * Gets or sets a failure value. Only the first success or failure value is accepted, subsequent calls are ignored. The getter must not be called unless the Promise has completed with a failure.
 */
@property (nonatomic, strong) FailureType failureValue;

#pragma mark - Callback Registration

/**
 * Adds a callback to be used then the Promise completes, regardless of success or failure.
 * If the Promise has already completed this callback will be called immediately.
 */
- (void)addCallback:(PromiseCompleteCallback)callback;

/**
 * Adds a callback to be used in case the Promise succeeds. If the Promise has already succeeded this callback will be called immediately.
 */
- (void)addSuccessCallback:(PromiseSuccessCallback)callback;

/**
 * Adds a callback to be used in case the Promise fails. If the Promise has already failed this callback will be called immediately.
 */
- (void)addFailureCallback:(PromiseFailureCallback)callback;


#pragma mark - Immediate completed promises

+ (ALPromise<SuccessType, FailureType> *)success:(SuccessType)successValue;
+ (ALPromise<SuccessType, FailureType> *)failure:(FailureType)failureValue;

#pragma mark - Initialization

- (instancetype)initWithName:(NSString *)name;

+ (instancetype)new NS_UNAVAILABLE;

@end

NS_ASSUME_NONNULL_END
