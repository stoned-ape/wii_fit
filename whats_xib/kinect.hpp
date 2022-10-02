#pragma once
#include <stdint.h>

struct kinect{
    virtual void update()=0;
    virtual float *get_depth_buf()=0;
    virtual float *get_ir_buf()=0;
    virtual void *get_rgb_buf()=0;
    virtual ~kinect();
};

kinect *make_kinect();


