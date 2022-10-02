//
// PoseNetMobileNet100S16FP16.m
//
// This file was automatically generated and should not be edited.
//

#import "PoseNetMobileNet100S16FP16.h"

@implementation PoseNetMobileNet100S16FP16Input

- (instancetype)initWithImage:(CVPixelBufferRef)image {
    self = [super init];
    if (self) {
        _image = image;
        CVPixelBufferRetain(_image);
    }
    return self;
}

- (void)dealloc {
    CVPixelBufferRelease(_image);
}

- (nullable instancetype)initWithImageFromCGImage:(CGImageRef)image error:(NSError * _Nullable __autoreleasing * _Nullable)error {
    if (self) {
        NSError *localError;
        BOOL result = YES;
        id retVal = nil;
        @autoreleasepool {
            do {
                MLFeatureValue * __image = [MLFeatureValue featureValueWithCGImage:image pixelsWide:513 pixelsHigh:513 pixelFormatType:kCVPixelFormatType_32ARGB options:nil error:&localError];
                if (__image == nil) {
                    result = NO;
                    break;
                }
                retVal = [self initWithImage:(CVPixelBufferRef)__image.imageBufferValue];
            }
            while(0);
        }
        if (error != NULL) {
            *error = localError;
        }
        return result ? retVal : nil;
    }
    return self;
}

- (nullable instancetype)initWithImageAtURL:(NSURL *)imageURL error:(NSError * _Nullable __autoreleasing * _Nullable)error {
    if (self) {
        NSError *localError;
        BOOL result = YES;
        id retVal = nil;
        @autoreleasepool {
            do {
                MLFeatureValue * __image = [MLFeatureValue featureValueWithImageAtURL:imageURL pixelsWide:513 pixelsHigh:513 pixelFormatType:kCVPixelFormatType_32ARGB options:nil error:&localError];
                if (__image == nil) {
                    result = NO;
                    break;
                }
                retVal = [self initWithImage:(CVPixelBufferRef)__image.imageBufferValue];
            }
            while(0);
        }
        if (error != NULL) {
            *error = localError;
        }
        return result ? retVal : nil;
    }
    return self;
}

-(BOOL)setImageWithCGImage:(CGImageRef)image error:(NSError * _Nullable __autoreleasing * _Nullable)error {
    NSError *localError;
    BOOL result = NO;
    @autoreleasepool {
        MLFeatureValue * __image = [MLFeatureValue featureValueWithCGImage:image pixelsWide:513 pixelsHigh:513 pixelFormatType:kCVPixelFormatType_32ARGB options:nil error:&localError];
        if (__image != nil) {
            CVPixelBufferRelease(_image);
            _image =  (CVPixelBufferRef)__image.imageBufferValue;
            CVPixelBufferRetain(_image);
            result = YES;
        }
    }
    if (error != NULL) {
        *error = localError;
    }
    return result;
}

-(BOOL)setImageWithURL:(NSURL *)imageURL error:(NSError * _Nullable __autoreleasing * _Nullable)error {
    NSError *localError;
    BOOL result = NO;
    @autoreleasepool {
        MLFeatureValue * __image = [MLFeatureValue featureValueWithImageAtURL:imageURL pixelsWide:513 pixelsHigh:513 pixelFormatType:kCVPixelFormatType_32ARGB options:nil error:&localError];
        if (__image != nil) {
            CVPixelBufferRelease(_image);
            _image =  (CVPixelBufferRef)__image.imageBufferValue;
            CVPixelBufferRetain(_image);
            result = YES;
        }
    }
    if (error != NULL) {
        *error = localError;
    }
    return result;
}

- (NSSet<NSString *> *)featureNames {
    return [NSSet setWithArray:@[@"image"]];
}

- (nullable MLFeatureValue *)featureValueForName:(NSString *)featureName {
    if ([featureName isEqualToString:@"image"]) {
        return [MLFeatureValue featureValueWithPixelBuffer:_image];
    }
    return nil;
}

@end

@implementation PoseNetMobileNet100S16FP16Output

- (instancetype)initWithDisplacementBwd:(MLMultiArray *)displacementBwd displacementFwd:(MLMultiArray *)displacementFwd offsets:(MLMultiArray *)offsets heatmap:(MLMultiArray *)heatmap {
    self = [super init];
    if (self) {
        _displacementBwd = displacementBwd;
        _displacementFwd = displacementFwd;
        _offsets = offsets;
        _heatmap = heatmap;
    }
    return self;
}

- (NSSet<NSString *> *)featureNames {
    return [NSSet setWithArray:@[@"displacementBwd", @"displacementFwd", @"offsets", @"heatmap"]];
}

- (nullable MLFeatureValue *)featureValueForName:(NSString *)featureName {
    if ([featureName isEqualToString:@"displacementBwd"]) {
        return [MLFeatureValue featureValueWithMultiArray:_displacementBwd];
    }
    if ([featureName isEqualToString:@"displacementFwd"]) {
        return [MLFeatureValue featureValueWithMultiArray:_displacementFwd];
    }
    if ([featureName isEqualToString:@"offsets"]) {
        return [MLFeatureValue featureValueWithMultiArray:_offsets];
    }
    if ([featureName isEqualToString:@"heatmap"]) {
        return [MLFeatureValue featureValueWithMultiArray:_heatmap];
    }
    return nil;
}

@end

@implementation PoseNetMobileNet100S16FP16


/**
    URL of the underlying .mlmodelc directory.
*/
+ (nullable NSURL *)URLOfModelInThisBundle {
    NSString *assetPath = [[NSBundle bundleForClass:[self class]] pathForResource:@"PoseNetMobileNet100S16FP16" ofType:@"mlmodelc"];
    if (nil == assetPath) { os_log_error(OS_LOG_DEFAULT, "Could not load PoseNetMobileNet100S16FP16.mlmodelc in the bundle resource"); return nil; }
    return [NSURL fileURLWithPath:assetPath];
}


/**
    Initialize PoseNetMobileNet100S16FP16 instance from an existing MLModel object.

    Usually the application does not use this initializer unless it makes a subclass of PoseNetMobileNet100S16FP16.
    Such application may want to use `-[MLModel initWithContentsOfURL:configuration:error:]` and `+URLOfModelInThisBundle` to create a MLModel object to pass-in.
*/
- (instancetype)initWithMLModel:(MLModel *)model {
    self = [super init];
    if (!self) { return nil; }
    _model = model;
    if (_model == nil) { return nil; }
    return self;
}


/**
    Initialize PoseNetMobileNet100S16FP16 instance with the model in this bundle.
*/
- (nullable instancetype)init {
    return [self initWithContentsOfURL:(NSURL * _Nonnull)self.class.URLOfModelInThisBundle error:nil];
}


/**
    Initialize PoseNetMobileNet100S16FP16 instance with the model in this bundle.

    @param configuration The model configuration object
    @param error If an error occurs, upon return contains an NSError object that describes the problem. If you are not interested in possible errors, pass in NULL.
*/
- (nullable instancetype)initWithConfiguration:(MLModelConfiguration *)configuration error:(NSError * _Nullable __autoreleasing * _Nullable)error {
    return [self initWithContentsOfURL:(NSURL * _Nonnull)self.class.URLOfModelInThisBundle configuration:configuration error:error];
}


/**
    Initialize PoseNetMobileNet100S16FP16 instance from the model URL.

    @param modelURL URL to the .mlmodelc directory for PoseNetMobileNet100S16FP16.
    @param error If an error occurs, upon return contains an NSError object that describes the problem. If you are not interested in possible errors, pass in NULL.
*/
- (nullable instancetype)initWithContentsOfURL:(NSURL *)modelURL error:(NSError * _Nullable __autoreleasing * _Nullable)error {
    MLModel *model = [MLModel modelWithContentsOfURL:modelURL error:error];
    if (model == nil) { return nil; }
    return [self initWithMLModel:model];
}


/**
    Initialize PoseNetMobileNet100S16FP16 instance from the model URL.

    @param modelURL URL to the .mlmodelc directory for PoseNetMobileNet100S16FP16.
    @param configuration The model configuration object
    @param error If an error occurs, upon return contains an NSError object that describes the problem. If you are not interested in possible errors, pass in NULL.
*/
- (nullable instancetype)initWithContentsOfURL:(NSURL *)modelURL configuration:(MLModelConfiguration *)configuration error:(NSError * _Nullable __autoreleasing * _Nullable)error {
    MLModel *model = [MLModel modelWithContentsOfURL:modelURL configuration:configuration error:error];
    if (model == nil) { return nil; }
    return [self initWithMLModel:model];
}


/**
    Construct PoseNetMobileNet100S16FP16 instance asynchronously with configuration.
    Model loading may take time when the model content is not immediately available (e.g. encrypted model). Use this factory method especially when the caller is on the main thread.

    @param configuration The model configuration
    @param handler When the model load completes successfully or unsuccessfully, the completion handler is invoked with a valid PoseNetMobileNet100S16FP16 instance or NSError object.
*/
+ (void)loadWithConfiguration:(MLModelConfiguration *)configuration completionHandler:(void (^)(PoseNetMobileNet100S16FP16 * _Nullable model, NSError * _Nullable error))handler {
    [self loadContentsOfURL:(NSURL * _Nonnull)[self URLOfModelInThisBundle]
              configuration:configuration
          completionHandler:handler];
}


/**
    Construct PoseNetMobileNet100S16FP16 instance asynchronously with URL of .mlmodelc directory and optional configuration.

    Model loading may take time when the model content is not immediately available (e.g. encrypted model). Use this factory method especially when the caller is on the main thread.

    @param modelURL The model URL.
    @param configuration The model configuration
    @param handler When the model load completes successfully or unsuccessfully, the completion handler is invoked with a valid PoseNetMobileNet100S16FP16 instance or NSError object.
*/
+ (void)loadContentsOfURL:(NSURL *)modelURL configuration:(MLModelConfiguration *)configuration completionHandler:(void (^)(PoseNetMobileNet100S16FP16 * _Nullable model, NSError * _Nullable error))handler {
    [MLModel loadContentsOfURL:modelURL
                 configuration:configuration
             completionHandler:^(MLModel *model, NSError *error) {
        if (model != nil) {
            PoseNetMobileNet100S16FP16 *typedModel = [[PoseNetMobileNet100S16FP16 alloc] initWithMLModel:model];
            handler(typedModel, nil);
        } else {
            handler(nil, error);
        }
    }];
}

- (nullable PoseNetMobileNet100S16FP16Output *)predictionFromFeatures:(PoseNetMobileNet100S16FP16Input *)input error:(NSError * _Nullable __autoreleasing * _Nullable)error {
    return [self predictionFromFeatures:input options:[[MLPredictionOptions alloc] init] error:error];
}

- (nullable PoseNetMobileNet100S16FP16Output *)predictionFromFeatures:(PoseNetMobileNet100S16FP16Input *)input options:(MLPredictionOptions *)options error:(NSError * _Nullable __autoreleasing * _Nullable)error {
    id<MLFeatureProvider> outFeatures = [_model predictionFromFeatures:input options:options error:error];
    if (!outFeatures) { return nil; }
    return [[PoseNetMobileNet100S16FP16Output alloc] initWithDisplacementBwd:(MLMultiArray *)[outFeatures featureValueForName:@"displacementBwd"].multiArrayValue displacementFwd:(MLMultiArray *)[outFeatures featureValueForName:@"displacementFwd"].multiArrayValue offsets:(MLMultiArray *)[outFeatures featureValueForName:@"offsets"].multiArrayValue heatmap:(MLMultiArray *)[outFeatures featureValueForName:@"heatmap"].multiArrayValue];
}

- (nullable PoseNetMobileNet100S16FP16Output *)predictionFromImage:(CVPixelBufferRef)image error:(NSError * _Nullable __autoreleasing * _Nullable)error {
    PoseNetMobileNet100S16FP16Input *input_ = [[PoseNetMobileNet100S16FP16Input alloc] initWithImage:image];
    return [self predictionFromFeatures:input_ error:error];
}

- (nullable NSArray<PoseNetMobileNet100S16FP16Output *> *)predictionsFromInputs:(NSArray<PoseNetMobileNet100S16FP16Input*> *)inputArray options:(MLPredictionOptions *)options error:(NSError * _Nullable __autoreleasing * _Nullable)error {
    id<MLBatchProvider> inBatch = [[MLArrayBatchProvider alloc] initWithFeatureProviderArray:inputArray];
    id<MLBatchProvider> outBatch = [_model predictionsFromBatch:inBatch options:options error:error];
    if (!outBatch) { return nil; }
    NSMutableArray<PoseNetMobileNet100S16FP16Output*> *results = [NSMutableArray arrayWithCapacity:(NSUInteger)outBatch.count];
    for (NSInteger i = 0; i < outBatch.count; i++) {
        id<MLFeatureProvider> resultProvider = [outBatch featuresAtIndex:i];
        PoseNetMobileNet100S16FP16Output * result = [[PoseNetMobileNet100S16FP16Output alloc] initWithDisplacementBwd:(MLMultiArray *)[resultProvider featureValueForName:@"displacementBwd"].multiArrayValue displacementFwd:(MLMultiArray *)[resultProvider featureValueForName:@"displacementFwd"].multiArrayValue offsets:(MLMultiArray *)[resultProvider featureValueForName:@"offsets"].multiArrayValue heatmap:(MLMultiArray *)[resultProvider featureValueForName:@"heatmap"].multiArrayValue];
        [results addObject:result];
    }
    return results;
}

@end
