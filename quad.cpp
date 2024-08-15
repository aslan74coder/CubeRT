#include "quad.h"

// рисование quad без отсечения

void HLine(int* p,int w,int l,int r,int color) {
    if(l>r) swap(l,r);
    l=max(l,0);
    r=min(r,w-1);
    for(p+=l,(r-=l)++;--r>=0;++p) *p=color;
}

void DrawTri(int* pix,int w,int h,vec2i a,vec2i b,vec2i c,int color) {
    if(a.y>b.y) swap(a,b);
    if(b.y>c.y) swap(b,c);
    if(a.y>b.y) swap(a,b);
    if(a.y==c.y)
;//        HLine(pix+w*a.y,w,a.x,a.x,color);
    else {

    int y=a.y;
    float l=a.x,r=l;
    float dl=(b.y>y)?flt(b.x-l)/(b.y-y):0;
    float dr=(c.y>y)?flt(c.x-l)/(c.y-y):0;
    for(int* p=pix+w*y;y<=b.y;++y,l+=dl,r+=dr,p+=w)
        if(y>=0&&y<h)
            HLine(p,w,l,r,color);
    y=c.y;l=r=c.x;dl=(y!=b.y)?flt(b.x-l)/(b.y-y):0;
    for(int* p=pix+w*y;y> b.y;--y,l-=dl,r-=dr,p-=w)
        if(y>=0&&y<h)
            HLine(p,w,l,r,color);
    }
}

void DrawQuad(int* p,int w,int h,vec2i a,vec2i b,vec2i c,vec2i d,int color) {
    DrawTri(p,w,h,a,b,c,color);
    DrawTri(p,w,h,a,c,d,color);
}

