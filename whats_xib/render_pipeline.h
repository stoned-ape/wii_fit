#pragma once

#define KINECT
_Pragma("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
_Pragma("GCC diagnostic ignored \"-Wunknown-attributes\"");


#ifdef __METAL_MACOS__ // GPU
#include <metal_stdlib>
using namespace metal;
constant float pi=3.141592653589793;
#define fmodf fmod

#else // CPU
//#define __AE__
//OBJC
//#import <Cocoa/Cocoa.h>
#import <Metal/Metal.h>
//#import <MetalKit/MetalKit.h>
//C STD
#include <sys/timeb.h>
#include <assert.h>
//GLM
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat2x2.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/geometric.hpp>
//C++ STD
#include <string>
#include <map>
//CUSTOM
#ifdef KINECT
#include "kinect.hpp"
kinect *kinect_dev=NULL;
__attribute__((constructor)) void dev_init(){
    kinect_dev=make_kinect();
}
__attribute__((destructor)) void dev_deinit(){
    delete kinect_dev;
}
#endif


using namespace glm;



typedef vec2 float2;
typedef vec3 packed_float3;
typedef vec4 float4;

static_assert(sizeof(packed_float3)==4*3,"");

typedef ivec2 int2;
typedef ivec3 packed_int3;
typedef ivec4 int4;

typedef mat2 float2x2;
typedef mat3 float3x3;
typedef mat4 float4x4;


#define SYSCALL_NOEXIT(call)({ \
    int syscall_ret=call; \
    if(syscall_ret==-1){ \
        fprintf(stderr,"syscall error: (%s) in function %s at line %d of file %s\n", \
            strerror(errno),__func__,__LINE__,__FILE__); \
        fprintf(stderr,"-> SYSCALL(%s)\n",#call); \
    } \
    syscall_ret; \
})

//exits on error
#define SYSCALL(call)({ \
    int syscall_ret=SYSCALL_NOEXIT(call); \
    if(syscall_ret==-1) exit(errno); \
    syscall_ret; \
})

//cpu should ignore address space qualifiers
#define thread
#define device
#define constant const

typedef int atomic_int;

#define PRINT(val,type) printf("%s:\t" type "\n",#val,val);

float itime(){
    struct timeb now;
    ftime(&now);
    return (float)(now.time%(60*60*24))+now.millitm/1e3;
}

void print_fps(){
    static float timer;
    float delta=itime()-timer;
    timer+=delta;
    printf("\rfps = %f ",1/delta);
    fflush(stdout);
}

#define TIME(code...)({\
    float __t0=itime();\
    code;\
    float __t1=itime();\
    float __diff=__t1-__t0;\
    printf("\rfreq = %.10f Hz,\time = %.10f ",1/__diff,__diff);\
    fflush(stdout);\
})


struct metal_gpu_internal{
    id<MTLBuffer> mtl_bufs[31];
    id<MTLDevice> _device;
    id<MTLCommandQueue> queue;
    id<MTLLibrary> library;
    id<MTLTexture> texture;
    id<MTLDrawable> drawable;
    struct func_data{
        id<MTLFunction> func;
        id<MTLComputePipelineState> state;
    };
    std::map<std::string,func_data> funcs;
    metal_gpu_internal(const metal_gpu_internal&){assert(0);};
    metal_gpu_internal &operator=(const metal_gpu_internal&){assert(0);};
    metal_gpu_internal(){
        @autoreleasepool{
            for(int i=0;i<31;i++) mtl_bufs[i]=NULL;
            _device=MTLCreateSystemDefaultDevice();
            assert(_device);
            NSLog(@"%@",_device.name);
            assert([_device hasUnifiedMemory]);
            queue=[_device newCommandQueue];
            assert(queue);
            NSBundle *main=[NSBundle mainBundle];
            NSString *nslib=[main pathForResource:@"default" ofType:@"metallib"];
            assert(nslib);
            NSURL *url=[NSURL fileURLWithPath:nslib];
            assert(url);
            NSError *error;
            library=[_device newLibraryWithURL:url error:&error];
            if(!library) NSLog(@"Error: %@\n",[error.userInfo objectForKey:@"NSLocalizedDescription"]);
            puts(getwd(NULL));
            assert(library);
        }
    }
    void *malloc(size_t size,int buf_idx){
        assert(buf_idx>=0);
        assert(buf_idx<31);
        assert(mtl_bufs[buf_idx]==NULL);
        mtl_bufs[buf_idx]=[_device newBufferWithLength:size options:0];
        void *ptr=[mtl_bufs[buf_idx] contents];
        assert(ptr);
        return ptr;
    }
    void call(const char *name,uint32_t x,uint32_t y,uint32_t z){
        @autoreleasepool{
            assert(name);
            assert(x>0);
            assert(y>0);
            assert(z>0);
            std::string sname=name;
            id<MTLFunction> func=NULL;
            id<MTLComputePipelineState> state=NULL;
            try{
                auto data=funcs.at(sname);
                func=data.func;
                state=data.state;
            }catch(std::out_of_range){
                NSString *nsname=[[NSString alloc] initWithUTF8String:name];
                assert(nsname);
                func=[library newFunctionWithName:nsname];
                assert(func);
                state=[_device newComputePipelineStateWithFunction:func error:NULL];
                assert(state);
                func_data fd={func,state};
                funcs[sname]=fd;
            }
            assert(func);
            id<MTLCommandBuffer> command_buffer=[queue commandBuffer];
            assert(command_buffer);
            id<MTLComputeCommandEncoder> command_encoder=[command_buffer computeCommandEncoder];
            assert(command_encoder);
            [command_encoder setComputePipelineState:state];
            
            for(int i=0;i<31;i++) if(mtl_bufs[i]){
                [command_encoder setBuffer:mtl_bufs[i] offset:0 atIndex:i];
            }
            [command_encoder setTexture:texture atIndex:0];
            
            
            MTLSize block_size=MTLSizeMake(1,1,1);
            [[maybe_unused]]uint64_t exw=state.threadExecutionWidth;
            [[maybe_unused]]uint64_t max=state.maxTotalThreadsPerThreadgroup;
            
//            if(y>1){
//                block_size=MTLSizeMake(exw,max/exw,1);
//            }else{
//                block_size=MTLSizeMake(exw,1,1);
//            }
            static auto rdiv=[](uint64_t a,uint64_t b)->uint64_t{
                if(a%b==0) return a/b;
                return a/b+1;
            };
            MTLSize grid_size=MTLSizeMake(rdiv(x,block_size.width),
                                          rdiv(y,block_size.height),
                                          rdiv(z,block_size.depth));
//            PRINT(exw,"%d");
//            PRINT(max,"%d");

            
            [command_encoder dispatchThreadgroups:grid_size threadsPerThreadgroup:block_size];
            [command_encoder endEncoding];
            if(strcmp(name,"display")==0) [command_buffer presentDrawable:drawable];
            [command_buffer commit];
            if(strcmp(name,"display")!=0) [command_buffer waitUntilCompleted];
            
            
        }
    }
};

#endif

static_assert(sizeof(packed_int3)==4*3,"");
static_assert(sizeof(packed_float3)==4*3,"");
static_assert(sizeof(float4x4)==4*4*4,"");


typedef struct{
    uint8_t b,g,r,a;
}bgra8;


float4x4 rotx(float theta){
    float _rot[4*4]={
        1,0,0,0,
        0,+cos(theta),sin(theta),0,
        0,-sin(theta),cos(theta),0,
        0,0,0,1,
    };
    return *(thread float4x4*)&_rot;
}

float4x4 roty(float theta){
    float _rot[4*4]={
        +cos(theta),0,sin(theta),0,
        0,1,0,0,
        -sin(theta),0,cos(theta),0,
        0,0,0,1,
    };
    return *(thread float4x4*)&_rot;
}

float4x4 rotz(float theta){
    float _rot[4*4]={
        +cos(theta),sin(theta),0,0,
        -sin(theta),cos(theta),0,0,
        0,0,1,0,
        0,0,0,1,
    };
    return *(thread float4x4*)&_rot;
}

template<typename floatnxn,int n>
floatnxn m_inv_template(floatnxn x){
    //initialize data structure of 2 matrices side by side
    //the first is x and the second is the identity matrix
    float data_raw[n][2*n];
    thread float *data[n];
    for(int i=0;i<n;i++) data[i]=&data_raw[i][0];
    for(int i=0;i<n;i++){
        for(int j=0;j<2*n;j++){
            if(j<n) data[i][j]=x[i][j];
            else data[i][j]=(i==j-n)?1:0;
        }
    }
    //make the first matrix diagonal
    for(int i=0;i<n;i++){
        if(data[i][i]==0){
            bool b=false;
            for(int j=i+1;j<n;j++){
                if(data[j][i]!=0){
                    thread float *tmp=data[i];
                    data[i]=data[j];
                    data[j]=tmp;
                    b=true;
                    break;
                }
            }
        }
        for(int j=i+1;j<n;j++){
            float coef=-data[j][i]/data[i][i];
            for(int k=0;k<2*n;k++) data[j][k]+=coef*data[i][k];
        }
    }
    //make the first matrix the identity matrix
    for(int i=n-1;i>0;i--){
        if(data[i][i]==0){
            bool b=false;
            for(int j=n-1;j>i;j--){
                if(data[j][i]!=0){
                    thread float *tmp=data[i];
                    data[i]=data[j];
                    data[j]=tmp;
                    b=true;
                    break;
                }
            }
        }
        for(int j=i-1;j>=0;j--){
            float coef=-data[j][i]/data[i][i];
            for(int k=0;k<2*n;k++) data[j][k]+=coef*data[i][k];
        }
    }
    for(int i=0;i<n;i++){
        for(int j=2*n-1;j>=n;j--){
            data[i][j]/=data[i][i];
        }
    }
    //the second matrix is now the inverse(x)
    //copy the second matrix into inv and return it
    floatnxn inv;
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            inv[i][j]=data[i][j+n];
        }
    }
    return inv;
}

float4x4 m_inv(float4x4 m){
    return m_inv_template<float4x4,4>(m);
}




struct __attribute__((packed)) _vertex{
    float4 v;
    float4 col;
    _vertex(float4 _v,float4 c):v(_v),col(c){}
};


static_assert(sizeof(_vertex)==8*4,"");


inline float pos(bool p){return p?1.0f:-1.0f;}

inline float map(float t,float t0,float t1,float s0,float s1){
    return s0+(s1-s0)*(t-t0)/(t1-t0);
}


float4 color_wheel(float t){
    float theta=map(t,0.,1.,0.,2.*pi);
    float modulus=2.0f*pi*.75f;
    theta=fmodf(theta,modulus)+2.*pi*.87;
    float2 angles=float2(cos(theta),sin(theta));
    angles.x=map(angles.x,-1.,1.,0.,pi/2.);
    angles.y=map(angles.y,-1.,1.,0.,pi/2.);
    return float4(cos(angles.x)*sin(angles.y),cos(angles.y),sin(angles.x)*sin(angles.y),1);
}


template<uint32_t capacity>
struct __attribute__((packed)) vertex_buffer{
    uint32_t size;
    _vertex data[capacity];
    device _vertex &operator[](int i){
        return data[i];
    }
    void push(_vertex v){
        if(size>=capacity) return;
        data[size]=v;
        size++;
    }
    void push_test_tri(){
        push(_vertex(float4(0,0,0,1),float4(0,1,1,1)));
        push(_vertex(float4(+.5f,+.5f,0,1),0.0f*float4(1,1,1,1)));
        push(_vertex(float4(+.5f,-.5f,0,1),0.0f*float4(1,1,1,1)));
    }
    void push_test_square(){
        push(_vertex(float4(-.5,-.5,0,1),float4(1,0,1,1)));
        push(_vertex(float4(-.5,+.5,0,1),float4(1,0,1,1)));
        push(_vertex(float4(+.5,-.5,0,1),float4(1,0,1,1)));
        push(_vertex(float4(-.5,+.5,0,1),float4(1,0,1,1)));
        push(_vertex(float4(+.5,-.5,0,1),float4(1,0,1,1)));
        push(_vertex(float4(+.5,+.5,0,1),float4(1,0,1,1)));
    }
    void push_test_cube(){
        for(int k=0;k<3;k++){
            float rgb[3]={(float)(k==0),(float)(k==1),(float)(k==2)};
            for(int j=0;j<2;j++){
                float4 col;
                if(j==0) col=float4(rgb[0],rgb[1],rgb[2],1);
                else col=float4(1-rgb[0],1-rgb[1],1-rgb[2],1);
                float vtx[3];
                vtx[k]=.5*pos(j);
                for(int i=0;i<3;i++){
                    vtx[(k+1)%3]=+.5*pos(i/2);
                    vtx[(k+2)%3]=+.5*pos(i%2);
                    push(_vertex(float4(vtx[0],vtx[1],vtx[2],1),col));
                }
                for(int i=1;i<4;i++){
                    vtx[(k+1)%3]=+.5*pos(i/2);
                    vtx[(k+2)%3]=+.5*pos(i%2);
                    push(_vertex(float4(vtx[0],vtx[1],vtx[2],1),col));
                }
            }
        }
    }
    void push_test_octa(){
        for(int i=0;i<8;i++){
            float4 col(i&1,i&2,i&4,1);
            push(_vertex(float4(.5*pos(i&1),0,0,1),col));
            push(_vertex(float4(0,.5*pos(i&2),0,1),col));
            push(_vertex(float4(0,0,.5*pos(i&4),1),col));
        }
    }
    void push_test_tetra(){
        const float a=sqrt(2.0f)/4;
        float4 points[4]={
            float4(+a,+.25,+0,1),
            float4(-a,+.25,+0,1),
            float4(+0,-.25,+a,1),
            float4(+0,-.25,-a,1),
        };
        for(int i=0;i<4;i++){
            float4 col=color_wheel(i/5.0+.1);
            for(int j=0;j<4;j++) if(i!=j){
                push(_vertex(points[j],col));
            }
        }
    }
    void push_test_sphere(int res){
        float inc=pi/res;
        float rho=.5;
        for(float phi=0;phi<pi;phi+=inc){
            for(float theta=0;theta<2*pi;theta+=inc){
                float4 col(
                    .5+.5*cos(theta+inc/2)*sin(phi+inc/2),
                    .5+.5*                 cos(phi+inc/2),
                    .5+.5*sin(theta+inc/2)*sin(phi+inc/2),1
                );
                if(phi!=0) for(int i=0;i<3;i++){
                    float t=theta+inc*(i&1);
                    float p=phi+.5*inc*(i&2);
                    float4 v(
                        rho*cos(t)*sin(p),
                        rho*cos(p),
                        rho*sin(t)*sin(p),
                        1
                    );
                    push(_vertex(v,col));
                }
                if(phi<pi-inc) for(int i=1;i<4;i++){
                    float t=theta+inc*(i&1);
                    float p=phi+.5*inc*(i&2);
                    float4 v(
                        rho*cos(t)*sin(p),
                        rho*cos(p),
                        rho*sin(t)*sin(p),
                        1
                    );
                    push(_vertex(v,col));
                }
            }
        }
    }
    void push_test_ico(){
        float a=2*sin(pi/5);
        float b=a*sin(acos(1/a));
        float d=2*sin(pi/10);
        float c=sqrt(a*a-d*d)/2;
        float s=.5;
        float4 ud[2]={s*float4(0,b+c,0,1),s*float4(0,-b-c,0,1)};
        float4 mid[2][5];
        for(int i=0;i<2;i++){
            for(int j=0;j<5;j++){
                float theta=2*pi*j/5.0+pi/5.0*i;
                mid[i][j]=s*float4(cos(theta),-pos(i)*c,sin(theta),1);
            }
        }
        for(int i=0;i<2;i++){
            for(int j=0;j<5;j++){
                float4 col=color_wheel(j/6.0+i*1/10.0);
                push(_vertex(mid[i][j],col));
                push(_vertex(mid[i][(j+1)%5],col));
                push(_vertex(ud[i],col));
            }
        }
        for(int j=0;j<5;j++){
            for(int i=0;i<2;i++){
                float4 col=color_wheel(j/6.0+1/20.0+1/40.0*i);
                push(_vertex(mid[ i][j],col));
                push(_vertex(mid[ i][(j+(i?4:1))%5],col));
                push(_vertex(mid[!i][j],col));
            }
        }
    }
    void push_clip_test(float a){
        push(_vertex(a*float4(0,0,0,1),float4(1,0,0,1)));
        push(_vertex(a*float4(-2,.5,0,1),float4(1,0,0,1)));
        push(_vertex(a*float4(-2,-.5,0,1),float4(1,0,0,1)));
        
        push(_vertex(a*float4(2,0,0,1),float4(1,0,0,1)));
        push(_vertex(a*float4(0,.5,0,1),float4(1,0,0,1)));
        push(_vertex(a*float4(0,-.5,0,1),float4(1,0,0,1)));
    }
};

typedef struct{
    _vertex v[3];
}triangle;

struct window;


#define WIDTH (512*2)
#define HEIGHT (424*2)
#define NUM_VERTS (300*4)
constant uint32_t w=WIDTH,h=HEIGHT,num_verts=NUM_VERTS;
constant float aspect=WIDTH/(float)HEIGHT;

#ifdef KINECT
#define K_WIDTH (512)
#define K_HEIGHT (424)
#define K_RGB_WIDTH (1920)
#define K_RGB_HEIGHT (1080)
#endif

#define FTC_WIDTH (640*2)
#define FTC_HEIGHT (480*2)

#define PN_WIDTH (513)
#define PN_HEIGHT (513)

union depth_ent{
    struct{
        int16_t index,depth;
    };
    int bits;
};

//typedef struct{
//    uint8_t idx1:4,idx2:4;
//}vga_pixel;
//static_assert(sizeof(vga_pixel)==1,"");

struct __attribute__((packed)) render_pipline{
//    static_assert(WIDTH%32==0,"");
//    static_assert(HEIGHT%32==0,"");
    static_assert(NUM_VERTS%3==0,"");
    vertex_buffer<NUM_VERTS> vertex_buf;
    vertex_buffer<NUM_VERTS> internal_vertex_buf;
    float4 pixel_buf[HEIGHT][WIDTH];
    depth_ent depth_buf[HEIGHT][WIDTH];
#ifdef KINECT
    float k_depth_buf[K_HEIGHT][K_WIDTH];
    bgra8 k_pixel_buf[K_RGB_HEIGHT][K_RGB_WIDTH];
#endif
    bgra8 ftc_pixel_buf[FTC_HEIGHT][FTC_WIDTH];
    bgra8 pn_pixel_buf[PN_HEIGHT][PN_WIDTH];
    int2 mouse;
    bool left_mouse_down;
    bool right_mouse_down;
    bool key_down[128];
    float time;
    uint32_t sizeof_this;
#ifndef __METAL_MACOS__
#ifdef KINECT
    void kinect_run(){
        kinect_dev->update();
        memcpy(k_depth_buf,kinect_dev->get_depth_buf(),K_WIDTH*K_HEIGHT*sizeof(float));
        memcpy(k_pixel_buf,kinect_dev->get_rgb_buf(),K_RGB_WIDTH*K_RGB_HEIGHT*sizeof(float));
    }
#endif
    void run(metal_gpu_internal *gpu){
        time=itime();
        sizeof_this=sizeof(render_pipline);
#ifdef KINECT
        kinect_run();
#endif
        
        gpu->call("per_pixel_kernel",w,h,1);
        gpu->call("per_vertex_kernel",vertex_buf.size,1,1);
        gpu->call("per_triangle_kernel",vertex_buf.size/3,3,2);
        gpu->call("fragment_kernel",w,h,1);
    }
#endif
};



device triangle *rp_get_triangle_buf(device render_pipline *_this [[buffer(0)]]){
    return (device triangle*)&_this->internal_vertex_buf.data;
}
int2 rp_norm2pix(device render_pipline *_this [[buffer(0)]],float2 uv){
    uv+=1;
    uv/=2;
    return int2(uv.x*w,uv.y*h);
}
float2 rp_pix2norm(device render_pipline *_this [[buffer(0)]],int2 idx){
    float2 uv=float2((float)idx.x/w,(float)idx.y/h);
    uv*=2;
    uv-=1;
    return uv;
}
void rp_set_pixel(device render_pipline *_this [[buffer(0)]],int2 idx,float4 col){
    _this->pixel_buf[idx.y][idx.x].r=col.x*255;
    _this->pixel_buf[idx.y][idx.x].g=col.y*255;
    _this->pixel_buf[idx.y][idx.x].b=col.z*255;
    _this->pixel_buf[idx.y][idx.x].a=col.w*255;
}





















