//CUSTOM
#include "render_pipeline.h"
#undef device
//C STD
#include <stdio.h>
#include <assert.h>
//OBJC
#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#import <AVFoundation/AVFoundation.h>
#import <CoreVideo/CoreVideo.h>
#import <VideoToolbox/VideoToolbox.h>
#import <CoreML/CoreML.h>
#import "PoseNetMobileNet100S16FP16.h"

@interface AppDelegate:NSObject<
    NSApplicationDelegate,
    MTKViewDelegate,
    AVCaptureVideoDataOutputSampleBufferDelegate>
@property (strong) NSWindow *window;
@property (strong) MTKView *view;
@property render_pipline *ren;
@property metal_gpu_internal gpu;
@property AVCaptureSession *capture_session;
@property AVCaptureVideoDataOutput *video_output;
@property dispatch_queue_t dq;
@property PoseNetMobileNet100S16FP16 *posenet;
@end

@implementation AppDelegate
-(void)applicationDidFinishLaunching:(NSNotification*)aNotification{
    puts("bruh");
    [[maybe_unused]]NSError *error;
    assert(_window);
    NSSize size;
    size.width=WIDTH/2;
    size.height=HEIGHT/2;
    [_window setContentSize:size];
    _view=[[MTKView alloc] init];
    _ren=(render_pipline*)_gpu.malloc(sizeof(render_pipline),0);
    
//    _ren->vertex_buf.push_test_tri();
//    _ren->vertex_buf.push_test_square();
//    _ren->vertex_buf.push_test_cube();
    _ren->vertex_buf.push_test_octa();
//    _ren->vertex_buf.push_test_tetra();
//    _ren->vertex_buf.push_test_sphere(10);
//    _ren->vertex_buf.push_test_ico();
//    _ren->vertex_buf.push_clip_test(1);

    
    _view.delegate=self;
    _view.device=_gpu._device;
    _view.framebufferOnly=false;
    _window.contentView=_view;
    
    _capture_session=[[AVCaptureSession alloc] init];
    assert(_capture_session);
    _video_output=[[AVCaptureVideoDataOutput alloc] init];
    assert(_video_output);
    
    [_capture_session beginConfiguration];
    _capture_session.sessionPreset=AVCaptureSessionPreset640x480;
    auto *capture_device=[AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeVideo];
    assert(capture_device);
    auto *video_input=[[AVCaptureDeviceInput alloc] initWithDevice:capture_device error:&error];
    assert(video_input);
    assert([_capture_session canAddInput:video_input]);
    [_capture_session addInput:video_input];
    
    _video_output.alwaysDiscardsLateVideoFrames=true;
    _video_output.videoSettings=@{(NSString*)kCVPixelBufferPixelFormatTypeKey:
                                      @(kCVPixelFormatType_420YpCbCr8BiPlanarFullRange)};;
    _dq=dispatch_queue_create("dispatch_queue",NULL);
    [_video_output setSampleBufferDelegate:self queue:_dq];

    assert([_capture_session canAddOutput:_video_output]);
    [_capture_session addOutput:_video_output];
    [_capture_session commitConfiguration];
    
    [_capture_session startRunning];
    assert(_capture_session.isRunning);
    
    
    
    _posenet=[[PoseNetMobileNet100S16FP16 alloc] init];
    assert(_posenet);
//    
////    __CVBuffer *ref=[];
//    PoseNetMobileNet100S16FP16Output *output=[pn predictionFromImage:ref error:&error];
    
    [NSEvent addLocalMonitorForEventsMatchingMask:NSEventMaskKeyDown
                                          handler:^(NSEvent *event){
        if(const char *c=event.characters.UTF8String){
            printf("key down: %c\n",c[0]);
            self->_ren->key_down[c[0]]=true;
        }
        return event;
    }];
    [NSEvent addLocalMonitorForEventsMatchingMask:NSEventMaskKeyUp
                                          handler:^(NSEvent *event){
        if(const char *c=event.characters.UTF8String){
            printf("key up: %c\n",c[0]);
            self->_ren->key_down[c[0]]=false;
        }
        return event;
    }];

    
    [NSEvent addLocalMonitorForEventsMatchingMask:NSEventMaskLeftMouseDown
                                          handler:^(NSEvent *event){
        self->_ren->left_mouse_down=true;
        return event;
    }];
    [NSEvent addLocalMonitorForEventsMatchingMask:NSEventMaskLeftMouseUp
                                          handler:^(NSEvent *event){
        self->_ren->left_mouse_down=false;
        return event;
    }];
    [NSEvent addLocalMonitorForEventsMatchingMask:NSEventMaskRightMouseDown
                                          handler:^(NSEvent *event){
        self->_ren->right_mouse_down=true;
        return event;
    }];
    [NSEvent addLocalMonitorForEventsMatchingMask:NSEventMaskRightMouseUp
                                          handler:^(NSEvent *event){
        self->_ren->right_mouse_down=false;
        return event;
    }];
}
-(BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender{
    return true;
}
-(void)applicationWillTerminate:(NSNotification *)notification{
    puts(__func__);
}
-(void)mtkView:(nonnull MTKView *)view drawableSizeWillChange:(CGSize)size{}
- (void)captureOutput:(AVCaptureOutput *)output didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer fromConnection:(AVCaptureConnection *)connection{
    
    CVPixelBufferRef buf=CMSampleBufferGetImageBuffer(sampleBuffer);
    CVPixelBufferLockBaseAddress(buf,kCVPixelBufferLock_ReadOnly);
    CGImageRef image=NULL;
    VTCreateCGImageFromCVPixelBuffer(buf,NULL,&image);
    assert(image);
    CVPixelBufferUnlockBaseAddress(buf,kCVPixelBufferLock_ReadOnly);
    CGDataProviderRef data_prov=CGImageGetDataProvider(image);
    CFDataRef data_ref=CGDataProviderCopyData(data_prov);
    void *data=(void*)CFDataGetBytePtr(data_ref);
    assert(data);
    uint64_t cpy_size=CGImageGetWidth(image)*CGImageGetHeight(image)*CGImageGetBitsPerPixel(image)/8;
    memcpy(&_ren->ftc_pixel_buf[0][0],data,cpy_size);
    _gpu.call("downsample",PN_WIDTH,PN_HEIGHT,1);
    CVPixelBufferRef pn_buf=NULL;
    CVPixelBufferCreateWithBytes(NULL,PN_WIDTH,PN_HEIGHT,
                                 kCMPixelFormat_32BGRA,
                                 &_ren->pn_pixel_buf[0][0],
                                 PN_WIDTH*sizeof(bgra8),
                                 NULL,NULL,NULL,&pn_buf);
    NSError *error=NULL;
    PoseNetMobileNet100S16FP16Output *pn_output=[_posenet predictionFromImage:pn_buf error:&error];
    if(!pn_output){
        NSLog(@"posenet error: %@s\n",error.description);
        assert(pn_output);
    }
    MLMultiArray *mlma=pn_output.offsets;
    assert(mlma.dataType==MLMultiArrayDataTypeDouble);
//    NSNumber
    assert(mlma.shape.count==3);
    assert(mlma.strides.count==3);
    int4 shape=int4(mlma.shape[0].intValue,mlma.shape[1].intValue,mlma.shape[2].intValue,0);
    int4 stride=int4(mlma.strides[0].intValue,mlma.strides[1].intValue,mlma.strides[2].intValue,0);
    PRINT(shape.x,"%d");
    PRINT(shape.y,"%d");
    PRINT(shape.z,"%d");
    PRINT(stride.x,"%d");
    PRINT(stride.y,"%d");
    PRINT(stride.z,"%d");
    
    struct data_layout{
        double data[17][33][33];
    };
//    static_assert(sizeof(data_layout)==34*33*33*sizeof(double),"");
    auto *pn_data=(data_layout*)mlma.dataPointer;
    
    
//    print_fps();

}
-(void)drawInMTKView:(nonnull MTKView *)view{
    _ren->mouse.x=NSEvent.mouseLocation.x-_window.frame.origin.x;
    _ren->mouse.y=NSEvent.mouseLocation.y-_window.frame.origin.y;
    _ren->run(&_gpu);
    _gpu.drawable=view.currentDrawable;
    _gpu.texture=view.currentDrawable.texture;
    _gpu.call("display",WIDTH,HEIGHT,1);
//    print_fps();
}
@end

int main(int argc,const char *argv[]){
    puts(__func__);
    return NSApplicationMain(argc,argv);
}

