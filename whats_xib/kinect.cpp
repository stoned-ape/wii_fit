#include "kinect.hpp"
#include <libfreenect2/libfreenect2.hpp>
#include <libfreenect2/frame_listener_impl.h>
#include <libfreenect2/registration.h>
#include <libfreenect2/packet_pipeline.h>


struct kinect_internal:kinect{
    libfreenect2::Freenect2 freenect2;
    std::string serial;
    libfreenect2::Freenect2Device *dev;
    libfreenect2::SyncMultiFrameListener *listener;
    libfreenect2::FrameMap frames;
    libfreenect2::PacketPipeline *pipline;
    kinect_internal(){
        assert(freenect2.enumerateDevices()!=0);
        serial=freenect2.getDefaultDeviceSerialNumber();
        puts("bruh");
        pipline=new libfreenect2::OpenCLPacketPipeline();
//        pipline=new libfreenect2::CpuPacketPipeline();
        dev=freenect2.openDevice(serial,pipline);
        assert(dev);
        listener=new libfreenect2::SyncMultiFrameListener(
            libfreenect2::Frame::Color|
            libfreenect2::Frame::Ir|
            libfreenect2::Frame::Depth
        );
        assert(listener);
        dev->setColorFrameListener(listener);
        dev->setIrAndDepthFrameListener(listener);
        assert(dev->start());
    }
    int itr=0;
    void update(){
        if(itr>0) listener->release(frames);
        assert(listener->waitForNewFrame(frames, 10*1000));
        itr++;
    }
    float *get_depth_buf(){
        return (float*)frames[libfreenect2::Frame::Depth]->data;
    }
    float *get_ir_buf(){
        return (float*)frames[libfreenect2::Frame::Ir]->data;
    }
    void *get_rgb_buf(){
        return (void*)frames[libfreenect2::Frame::Color]->data;
    }
    ~kinect_internal(){
        puts(__func__);
        dev->stop();
        dev->close();
        delete listener;
//        delete pipline
    }
};

kinect *make_kinect(){
    return new kinect_internal();
}

kinect::~kinect(){
    puts(__func__);
}







//

