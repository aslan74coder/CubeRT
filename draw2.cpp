#include "draw.h"
#include "draw.h"
#include "ray.h"
#include "quad.h"
//???
#include <ata/color.h>

#define minlev 4
int maxlev=9;

struct Frustum;

struct SRay {
    vec3f r;  // world coord (for tracing)
    vec3f l;  // view coord (for clipping)
    vec2i p;  // screen coord
    int   c;  // color picked
    bool  in; // in frustum
    SRay(const Frustum& f,const vec3f& _r,float t);
    SRay(const Frustum& f,const SRay& r1,const SRay& r2,float t);
};

struct Frustum {
    vec3f p,ex,ey,ez; // вершина и коорд оси
    int W,H;
    float cx,cy;
    Frustum(const mat34f& m,float w,float h,int W,int H);
    inline vec2i P(const vec3f& r) const { return vec2i(cx*(1.+r.x/r.z),cy*(1.+r.y/r.z)); }
};

Frustum::Frustum(const mat34f& m,float w,float h,int W,int H) {
    p=-m.pos;ex=1.f/w*m.rot[0];ey=1.f/h*m.rot[1];ez=m.rot[2];
    cx=0.5*W;cy=0.5*H;
    this->W=W;this->H=H;
};

SRay::SRay(const Frustum& f,const vec3f& _r,float t) {
    r=_r;
    l.x=dot(r,f.ex);
    l.y=dot(r,f.ey);
    l.z=dot(r,f.ez);
    in=(l.z>0&&fabs(l.x)<=l.z&&fabs(l.y)<=l.z);
    if(in) {
        c=Ray(f.p,r,t);
        p=f.P(l);
    }
};

SRay::SRay(const Frustum& f,const SRay& r1,const SRay& r2,float t) {
    r=0.5f*(r1.r+r2.r);
    l=0.5f*(r1.l+r2.l);
    in=(l.z>0&&fabs(l.x)<=l.z&&fabs(l.y)<=l.z);
    if(in) {
        c=(r1.in&&r2.in&&r1.c==r2.c)?r1.c:Ray(f.p,r,t);
        p=f.P(l);
    }
}

inline void DrawQuad(int* p,Frustum& f,const SRay& r1,const SRay& r2,const SRay& r3,const SRay& r4) {
    DrawQuad(p,f.W,f.H,r1.p,r2.p,r3.p,r4.p,Color(r1.c));
}

inline void SplitQuad(int* p,Frustum& f,const SRay& r1,const SRay& r2,const SRay& r3,const SRay& r4,int lev,float t);

void DrawQuad(int* p,Frustum& f,const SRay& r1,const SRay& r2,const SRay& r3,const SRay& r4,int lev,float t) {
    // 4 7 3
    // 8 9 6
    // 1 5 2
    if(lev<minlev||(r1.in|r2.in|r3.in|r4.in))
        if(r1.in&r2.in&r3.in&r4.in)
            if(lev>=maxlev||(r2.c==r1.c&r3.c==r1.c&r4.c==r1.c))
                DrawQuad(p,f,r1,r2,r3,r4);
            else
                SplitQuad(p,f,r1,r2,r3,r4,lev,t);
        else
        if(lev<maxlev)
            SplitQuad(p,f,r1,r2,r3,r4,lev,t);
}

inline void SplitQuad(int* p,Frustum& f,const SRay& r1,const SRay& r2,const SRay& r3,const SRay& r4,int lev,float t) {
    SRay r5(f,r1,r2,t),r6(f,r2,r3,t),r7(f,r3,r4,t),r8(f,r4,r1,t),r9(f,r5,r7,t);
    ++lev;
    DrawQuad(p,f,r1,r5,r9,r8,lev,t);
    DrawQuad(p,f,r5,r2,r6,r9,lev,t);
    DrawQuad(p,f,r8,r9,r7,r4,lev,t);
    DrawQuad(p,f,r9,r6,r3,r7,lev,t);
}

void DrawFace(int* p,Frustum& f,float t,int i,int j,int k,int s) {
    vec3f ei,ej,ek;ei[i]=ej[j]=ek[k]=s;
    DrawQuad(p,f,SRay(f,ek-ei-ej,t),SRay(f,ek+ei-ej,t),SRay(f,ek+ei+ej,t),SRay(f,ek-ei+ej,t),0,t);
}

void DrawCube(int* p,Frustum& f,float t) {
    for(int i=0,j=1,k=2;i<3;j=k,k=i++) { // 3 коорд оси
        DrawFace(p,f,t,i,j,k,-1);
        DrawFace(p,f,t,i,j,k,+1);
    }
}

void Draw(const mat34f& m,int W,int H,float fov,float zn,float zf,int* pix) {
    float w=tan(fov*M_PI/360),h=w*H/W; // пол-ширины, пол-высоты / расстояние до плоскости
    Frustum f(m,w,h,W,H);
    DrawCube(pix,f,zf);
}


/*

        if(allin)
            if(lev>=maxlev||alleq)
                DrawQuad(p,f,r1,r2,r3,r4);
            else
                SplitQuad(p,f,r1,r2,r3,r4,lev,t);
        else
        if(lev<maxlev)
            SplitQuad(p,f,r1,r2,r3,r4,lev,t);



        if allin && (lev>=maxlev || alleq)
            DrawQuad(p,f,r1,r2,r3,r4);
        if lev<maxlev
            if !allin || !alleq
                SplitQuad(p,f,r1,r2,r3,r4,lev,t);
*/
