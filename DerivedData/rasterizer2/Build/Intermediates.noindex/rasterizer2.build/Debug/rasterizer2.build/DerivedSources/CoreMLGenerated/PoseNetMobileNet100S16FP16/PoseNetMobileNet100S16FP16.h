//
// PoseNetMobileNet100S16FP16.h
//
// This file was automatically generated and should not be edited.
//

#import <Foundation/Foundation.h>
#import <CoreML/CoreML.h>
#include <stdint.h>
#include <os/log.h>

NS_ASSUME_NONNULL_BEGIN


/// Model Prediction Input Type
API_AVAILABLE(macos(10.14), ios(12.0), watchos(5.0), tvos(12.0)) __attribute__((visibility("hidden")))
@interface PoseNetMobileNet100S16FP16Input : NSObject<MLFeatureProvider>

/// RGB Image as color (kCVPixelFormatType_32BGRA) image buffer, 513 pixels wide by 513 pixels high
@property (readwrite, nonatomic) CVPixelBufferRef image;
- (instancetype)init NS_UNAVAILABLE;
- (instancetype)initWithImage:(CVPixelBufferRef)image NS_DESIGNATED_INITIALIZER;

- (nullable instancetype)initWithImageFromCGImage:(CGImageRef)image error:(NSError * _Nullable __autoreleasing * _Nullable)error API_AVAILABLE(macos(10.15), ios(13.0), watchos(6.0), tvos(13.0)) __attribute__((visibility("hidden")));

- (nullable instancetype)initWithImageAtURL:(NSURL *)imageURL error:(NSError * _Nullable __autoreleasing * _Nullable)error API_AVAILABLE(macos(10.15), ios(13.0), watchos(6.0), tvos(13.0)) __attribute__((visibility("hidden")));

-(BOOL)setImageWithCGImage:(CGImageRef)image error:(NSError * _Nullable __autoreleasing * _Nullable)error  API_AVAILABLE(macos(10.15), ios(13.0), watchos(6.0), tvos(13.0)) __attribute__((visibility("hidden")));
-(BOOL)setImageWithURL:(NSURL *)imageURL error:(NSError * _Nullable __autoreleasing * _Nullable)error  API_AVAILABLE(macos(10.15), ios(13.0), watchos(6.0), tvos(13.0)) __attribute__((visibility("hidden")));
@end


/// Model Prediction Output Type
API_AVAILABLE(macos(10.14), ios(12.0), watchos(5.0), tvos(12.0)) __attribute__((visibility("hidden")))
@interface PoseNetMobileNet100S16FP16Output : NSObject<MLFeatureProvider>

/// A multidimensional array that stores the displacement vector from each joint to its parent. as 32 × 33 × 33 3-dimensional array of doubles
@property (readwrite, nonatomic, strong) MLMultiArray * displacementBwd;

/// A multidimensional array that stores the displacement vector from each parent joint to one of its children. as 32 × 33 × 33 3-dimensional array of doubles
@property (readwrite, nonatomic, strong) MLMultiArray * displacementFwd;

/// A multidimensional array that stores an offset for each joint. as 34 × 33 × 33 3-dimensional array of doubles
@property (readwrite, nonatomic, strong) MLMultiArray * offsets;

/// A multidimensional array that stores the confidence for each joint. as 17 × 33 × 33 3-dimensional array of doubles
@property (readwrite, nonatomic, strong) MLMultiArray * heatmap;
- (instancetype)init NS_UNAVAILABLE;
- (instancetype)initWithDisplacementBwd:(MLMultiArray *)displacementBwd displacementFwd:(MLMultiArray *)displacementFwd offsets:(MLMultiArray *)offsets heatmap:(MLMultiArray *)heatmap NS_DESIGNATED_INITIALIZER;

@end


/// Class for model loading and prediction
API_AVAILABLE(macos(10.14), ios(12.0), watchos(5.0), tvos(12.0)) __attribute__((visibility("hidden")))
@interface PoseNetMobileNet100S16FP16 : NSObject
@property (readonly, nonatomic, nullable) MLModel * model;

/**
    URL of the underlying .mlmodelc directory.
*/
+ (nullable NSURL *)URLOfModelInThisBundle;

/**
    Initialize PoseNetMobileNet100S16FP16 instance from an existing MLModel object.

    Usually the application does not use this initializer unless it makes a subclass of PoseNetMobileNet100S16FP16.
    Such application may want to use `-[MLModel initWithContentsOfURL:configuration:error:]` and `+URLOfModelInThisBundle` to create a MLModel object to pass-in.
*/
- (instancetype)initWithMLModel:(MLModel *)model NS_DESIGNATED_INITIALIZER;

/**
    Initialize PoseNetMobileNet100S16FP16 instance with the model in this bundle.
*/
- (nullable instancetype)init;

/**
    Initialize PoseNetMobileNet100S16FP16 instance with the model in this bundle.

    @param configuration The model configuration object
    @param error If an error occurs, upon return contains an NSError object that describes the problem. If you are not interested in possible errors, pass in NULL.
*/
- (nullable instancetype)initWithConfiguration:(MLModelConfiguration *)configuration error:(NSError * _Nullable __autoreleasing * _Nullable)error;

/**
    Initialize PoseNetMobileNet100S16FP16 instance from the model URL.

    @param modelURL URL to the .mlmodelc directory for PoseNetMobileNet100S16FP16.
    @param error If an error occurs, upon return contains an NSError object that describes the problem. If you are not interested in possible errors, pass in NULL.
*/
- (nullable instancetype)initWithContentsOfURL:(NSURL *)modelURL error:(NSError * _Nullable __autoreleasing * _Nullable)error;

/**
    Initialize PoseNetMobileNet100S16FP16 instance from the model URL.

    @param modelURL URL to the .mlmodelc directory for PoseNetMobileNet100S16FP16.
    @param configuration The model configuration object
    @param error If an error occurs, upon return contains an NSError object that describes the problem. If you are not interested in possible errors, pass in NULL.
*/
- (nullable instancetype)initWithContentsOfURL:(NSURL *)modelURL configuration:(MLModelConfiguration *)configuration error:(NSError * _Nullable __autoreleasing * _Nullable)error;

/**
    Construct PoseNetMobileNet100S16FP16 instance asynchronously with configuration.
    Model loading may take time when the model content is not immediately available (e.g. encrypted model). Use this factory method especially when the caller is on the main thread.

    @param configuration The model configuration
    @param handler When the model load completes successfully or unsuccessfully, the completion handler is invoked with a valid PoseNetMobileNet100S16FP16 instance or NSError object.
*/
+ (void)loadWithConfiguration:(MLModelConfiguration *)configuration completionHandler:(void (^)(PoseNetMobileNet100S16FP16 * _Nullable model, NSError * _Nullable error))handler API_AVAILABLE(macos(11.0), ios(14.0), watchos(7.0), tvos(14.0)) __attribute__((visibility("hidden")));

/**
    Construct PoseNetMobileNet100S16FP16 instance asynchronously with URL of .mlmodelc directory and optional configuration.

    Model loading may take time when the model content is not immediately available (e.g. encrypted model). Use this factory method especially when the caller is on the main thread.

    @param modelURL The model URL.
    @param configuration The model configuration
    @param handler When the model load completes successfully or unsuccessfully, the completion handler is invoked with a valid PoseNetMobileNet100S16FP16 instance or NSError object.
*/
+ (void)loadContentsOfURL:(NSURL *)modelURL configuration:(MLModelConfiguration *)configuration completionHandler:(void (^)(PoseNetMobileNet100S16FP16 * _Nullable model, NSError * _Nullable error))handler API_AVAILABLE(macos(11.0), ios(14.0), watchos(7.0), tvos(14.0)) __attribute__((visibility("hidden")));

/**
    Make a prediction using the standard interface
    @param input an instance of PoseNetMobileNet100S16FP16Input to predict from
    @param error If an error occurs, upon return contains an NSError object that describes the problem. If you are not interested in possible errors, pass in NULL.
    @return the prediction as PoseNetMobileNet100S16FP16Output
*/
- (nullable PoseNetMobileNet100S16FP16Output *)predictionFromFeatures:(PoseNetMobileNet100S16FP16Input *)input error:(NSError * _Nullable __autoreleasing * _Nullable)error;

/**
    Make a prediction using the standard interface
    @param input an instance of PoseNetMobileNet100S16FP16Input to predict from
    @param options prediction options
    @param error If an error occurs, upon return contains an NSError object that describes the problem. If you are not interested in possible errors, pass in NULL.
    @return the prediction as PoseNetMobileNet100S16FP16Output
*/
- (nullable PoseNetMobileNet100S16FP16Output *)predictionFromFeatures:(PoseNetMobileNet100S16FP16Input *)input options:(MLPredictionOptions *)options error:(NSError * _Nullable __autoreleasing * _Nullable)error;

/**
    Make a prediction using the convenience interface
    @param image RGB Image as color (kCVPixelFormatType_32BGRA) image buffer, 513 pixels wide by 513 pixels high:
    @param error If an error occurs, upon return contains an NSError object that describes the problem. If you are not interested in possible errors, pass in NULL.
    @return the prediction as PoseNetMobileNet100S16FP16Output
*/
- (nullable PoseNetMobileNet100S16FP16Output *)predictionFromImage:(CVPixelBufferRef)image error:(NSError * _Nullable __autoreleasing * _Nullable)error;

/**
    Batch prediction
    @param inputArray array of PoseNetMobileNet100S16FP16Input instances to obtain predictions from
    @param options prediction options
    @param error If an error occurs, upon return contains an NSError object that describes the problem. If you are not interested in possible errors, pass in NULL.
    @return the predictions as NSArray<PoseNetMobileNet100S16FP16Output *>
*/
- (nullable NSArray<PoseNetMobileNet100S16FP16Output *> *)predictionsFromInputs:(NSArray<PoseNetMobileNet100S16FP16Input*> *)inputArray options:(MLPredictionOptions *)options error:(NSError * _Nullable __autoreleasing * _Nullable)error;
@end

NS_ASSUME_NONNULL_END
