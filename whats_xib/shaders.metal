#include "render_pipeline.h"

#define col_assert(e)({\
    if(!(e)){\
        for(int i=0;i<(int)w;i++){\
            for(int j=0;j<(int)h;j++){\
                rp_set_pixel(_this,int2(i,j),float4((i%2)?1:0,0,0,1));\
            }\
        }\
    }\
})


kernel void per_pixel_kernel(device render_pipline *_this [[buffer(0)]],
                                    uint3 idx [[thread_position_in_grid]]){
    if(idx.x>=w && idx.y>=h) return;
    _this->depth_buf[idx.y][idx.x].index=-1;
#ifdef KINECT
    device bgra8 *cam_pix=&_this->k_pixel_buf[(K_RGB_HEIGHT*idx.y)/HEIGHT][(K_RGB_WIDTH*idx.x)/WIDTH];
    float kdepth=_this->k_depth_buf[idx.y/2][idx.x/2];
    if(kdepth>.1){
        _this->depth_buf[idx.y][idx.x].depth=(1-kdepth/3000.0)*w;
        float color_fov=pi/3,depth_fov=pi/3;
        float depth_theta=asin(map(idx.x,0,w,sin(depth_fov/2),-sin(depth_fov/2)));
        float cam_dist=0; //mm
        
        float color_theta=pi/2-atan2(
            (kdepth*sin(pi/2-depth_theta)),
            (cam_dist-kdepth*cos(pi/2-depth_theta))
        );
//        color_theta=-depth_theta;
        int idx_x_adj=map(sin(color_theta),-sin(color_fov/2),sin(color_fov/2),0,K_RGB_WIDTH);
        cam_pix=&_this->k_pixel_buf[(K_RGB_HEIGHT*idx.y)/HEIGHT][idx_x_adj];
    }else{
        _this->depth_buf[idx.y][idx.x].depth=0;
    }
    float4 col(cam_pix->r/255.0,cam_pix->g/255.0,cam_pix->b/255.0,1);
#else
    _this->depth_buf[idx.y][idx.x].depth=-32768;
    float4 col=float4((float)idx.x/w,0,(float)idx.y/h,1)/4;
#endif
    
//    col=float4(_this->ftc_pixel_buf[idx.y][idx.x].r/255.0,
//               _this->ftc_pixel_buf[idx.y][idx.x].g/255.0,
//               _this->ftc_pixel_buf[idx.y][idx.x].b/255.0,1)/4;
    
    _this->pixel_buf[idx.y][idx.x]=col;
}

kernel void per_vertex_kernel(device render_pipline *_this [[buffer(0)]],
                                     uint3 idx [[thread_position_in_grid]]){
    if(idx.x>_this->vertex_buf.size) return;
    float theta=map(_this->mouse.x,0,w/2,pi,-pi);
    float phi  =map(_this->mouse.y,0,h/2,-pi/2,pi/2);
    float4x4 rot=rotx(phi)*roty(theta);
    _this->internal_vertex_buf.data[idx.x].v=rot*_this->vertex_buf.data[idx.x].v;

    _this->internal_vertex_buf.data[idx.x].col=_this->vertex_buf.data[idx.x].col;
}

bool eq(packed_float3 a,packed_float3 b){
    packed_float3 ad=abs(a-b);
    packed_bool3 c=ad<.1;
    return c.x&&c.y&&c.z;
}

kernel void per_triangle_kernel(device render_pipline *_this [[buffer(0)]],
                                       uint3 idx [[thread_position_in_grid]]){
    if(idx.x>_this->vertex_buf.size/3) return;
    triangle tri=*(rp_get_triangle_buf(_this)+idx.x);
    for(int i=0;i<3;i++){
        tri.v[i].v.x/=aspect;
        tri.v[i].v+=1;
        tri.v[i].v/=2;
        tri.v[i].v*=float4(w,h,w,1);
    }
    float3 ip=tri.v[(idx.y+0)%3].v.xyz;
    float3 jp=tri.v[(idx.y+1)%3].v.xyz;
    float3 kp=tri.v[(idx.y+2)%3].v.xyz;
    float3 jpr=jp-ip;
    float3 kpr=kp-ip;
    if(jpr.y*kpr.y<=0) return;
    float jslope=jpr.x/jpr.y;
    float kslope=kpr.x/kpr.y;
    float3 n=cross(jpr,kpr);
    float2 xrange=float2(min(min(jpr.x,kpr.x),0.0f),
                         max(max(jpr.x,kpr.x),0.0f));
    int dy=jpr.y>0?1:-1;
    int2 yrange=int2(0,(jpr.y>0?min(jpr.y,kpr.y):max(jpr.y,kpr.y))+2*dy);
    float s=rsqrt(2.0f);
    int ymid=round(yrange[1]*s);
    yrange[idx.z]=ymid;
    for(int y=yrange[0];y!=yrange[1];y+=dy){
        int _idxy=y+ip.y;
        if(_idxy>=(int)h || _idxy<0) continue;
        int xmin=max(min(y*jslope,y*kslope)-2,xrange.x);
        int xmax=min(max(y*jslope,y*kslope)+2,xrange.y);
        for(int x=xmin;x<xmax;x++){
            int2 _idx=int2(x+ip.x,_idxy);
            if(_idx.x>=(int)w || _idx.x<0) continue;
            float z=-(n.x*x+n.y*y)/n.z;
            int depth=ip.z+z;
            device auto *depth_ptr=(device atomic_int*)&_this->depth_buf[_idx.y][_idx.x].bits;
            depth_ent dent{{(int16_t)idx.x,(int16_t)depth}};
            atomic_fetch_max_explicit(depth_ptr,dent.bits,memory_order_relaxed);
        }
    }
}

kernel void fragment_kernel(device render_pipline *_this [[buffer(0)]],
                                   uint3 idx [[thread_position_in_grid]]){
    if(idx.x>=w && idx.y>=h) return;
    int tidx=_this->depth_buf[idx.y][idx.x].index;
    if(tidx==-1) return;
    device triangle *tri=rp_get_triangle_buf(_this)+tidx;
    float3 v1=tri->v[0].v.xyz,v2=tri->v[1].v.xyz,v3=tri->v[2].v.xyz;
    float4 col=tri->v[0].col;
    float3 n=normalize(cross(v2-v1,v3-v1));
    if(n.z<0) n*=-1;
    float3 lightd=normalize(float3(-1,-1,1));
    float3 ref=reflect(float3(0,0,-1),n);
    float ambient=.2;
    float lambert=clamp(dot(n,lightd),0.0f,1.0f);
    _this->pixel_buf[idx.y][idx.x]=col*(lambert+ambient);
}

kernel void display(device render_pipline *_this [[buffer(0)]],
                 texture2d<float,access::write> output [[texture(0)]],
                 uint3 idx [[thread_position_in_grid]]){
#if 1
    output.write(_this->pixel_buf[idx.y][idx.x],idx.xy);
#else
    float4 col;
    float div=w;
    int16_t depth=_this->depth_buf[idx.y][idx.x].depth;
    col.r=clamp(depth/div,0.0f,1.0f);
    col.g=clamp(depth/div,0.0f,1.0f);
    col.b=clamp(depth/div,0.0f,1.0f);
    col.a=1;
    output.write(col,idx.xy);
#endif
//    if(_this->_this!=(uint64_t)_this) output.write(float4(1,0,0,1),idx.xy);
    if(_this->sizeof_this!=sizeof(render_pipline)) output.write(float4(1,0,0,1),idx.xy);
    
//    output.write(color_wheel(idx.x/(float)output.get_width()),idx.xy);
}


kernel void downsample(device render_pipline *_this [[buffer(0)]],
                       uint3 idx [[thread_position_in_grid]]){
    if(idx.x>=PN_WIDTH && idx.y>=PN_HEIGHT) return;
    uint2 ftc_idx;
    ftc_idx.x=FTC_WIDTH/2;
    ftc_idx.y=FTC_HEIGHT/2;
    ftc_idx.x+=(ftc_idx.x-ftc_idx.y)/2;
    _this->pn_pixel_buf[idx.y][idx.x]=_this->ftc_pixel_buf[ftc_idx.y][ftc_idx.x];
}
