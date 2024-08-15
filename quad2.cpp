#include "quad.h"

using namespace ata;
using namespace vec;

float w,h,zn;

void HLine(int l,int r,int *p,int c) {
    for(l=max(l,0),r=min(r,w),p+=l,r-=l;--r>=0;++p) *p=c;
}

void DrawTrap(int u,int b,float ul,float ur,float bl,float br,float dl,float dr,int c) {
    if(u>=h||b<0||(ur<0&&br<0)||(ul>=w&&bl>=w)) return;
    float t=max(0f,-u);
    if(ur<0) t=max(t,-ur/dr);
    if(ul>=w) t=max(t,(w-ul)/dl);
    ul+=dl*t;ur+=dr*t;
    for(u+=t,p+=w*u; u<=b&&l<w&&r>=0; ++u,ul+=dl,ur+=dr,p+=w)
        HLine(ul,ur,p,c);
}

void DrawTri(vec2i v1,vec2i v2,vec2i v3,int c) {
    if(v1.y>v2.y) swap(v1,v2);
    if(v2.y>v3.y) swap(v2,v3);
    if(v1.y>v3.y) swap(v1,v3);
    if(v1.y>=h||v3.y<0) return;
    float dl=(v2.y>v1.y)?(v2.x-v1.x)/(v2.y-v1.y):0;
    float dr=(v3.y>v1.y)?(v3.x-v1.x)/(v3.y-v1.y):0;
    float dm=(v3.y>v2.y)?(v3.x-v2.x)/(v3.y-v2.y):0;
    float l=v2.x,r=v1.x+dr*(v2.y-v1.y);
    if(dl<=dr) {
        DrawTrap(v1.y,v2.y,v1.x,v1.x,l,r,dl,dr);
        DrawTrap(v2.y,v3.y,l,r,v3.x,v3.x,dm,dr);
    }
    else {
        DrawTrap(v1.y,v2.y,v1.x,v1.x,r,l,dr,dl);
        DrawTrap(v2.y,v3.y,r,l,v3.x,v3.x,dr,dm);
    }
}

vec3f MP(const vec3f& a,const vec3f& b) {
	float t=(zn-a.z)/(b.z-a.z);
    return vec3f(a.x+(b.x-a.x)*t,a.y+(b.y-a.y)*t,zn);
}

vec2i PR(const vec3f& v) {
    return vec2i(CX+K*v.x/v.z,CY+K*v.y/v.z)
}

// отсечь по znear и получ 0-3 треуг
void DrawQuad(vec3 v[4],int c) {
	vec2i w[5];
    int i=0,j,k=0;
    for(;i<4;i++)
    	if(v[i].z>=zn)
        	w[k++]=PR(v[i]);
        else {
        	j=(i+3)&3;if(v[j].z>=zn) w[k++]=PR(MP(v[i],v[j]));
            j=(i+1)&3;if(v[j].z>=zn) w[k++]=PR(MP(v[i],v[j]));
        }
    for(i=2;i<k;++i)
    	DrawTri(w[0],w[i-1],w[i]);
}

void DrawQuad(vec3f& a,vec3f& b,vec3f& c,int d) {
	vec3f v[4];
    for(int i=2,j,k=0;--i>=0;a=-a) {
    	for(j=2;--j>=0;b=-b)
        	v[k++]=a+b+c;
    DrawQuad(v,d);
}

/*???
void DrawQuad(vec2i v[4],int c) {
	int i=0,j=1,k=(v[0]<0);
    for(;j<4;++j) {
    	if(v[j].y<v[i].y)
        	i=j;
        k+=(v[j]<0);
    }
    if(v[j].y>=h||k==4) return;
    j=(i+3)&3;k=(i+1)&3;
    float dl=(v[j].y>v[i].y)?(v[j].x-v[i].x)/(v[j].y-v[i].y):0;
    float dr=(v[k].y>v[i].y)?(v[k].x-v[i].x)/(v[k].y-v[i].y):0;
    if(dl>dr) {
    	swap(dl,dr);
        swap(j,k);
    }
    float l=v[i].x,r=l;
    for() {
    	a
    }
}
*/

WeilerЦAtherton polygon clipping algorithm

 лип полигон P против клип полигона C
ќба вершины по CCW
получ все точки пересечени€, также сорт по CCW
встав их в P и C
точки пересечени€ дл€ стороны L идут между ее концами A и B в пор€дке возрастани€ t (от A к B)

ќбход вершины P
	если Pi inside C
    	добав Pi в list

bool Line2Line(const vec2f& a,const vec2f& b,const vec2f& c,const vec2f& d) {
    float A=par(a-c,d-c),B=par(b-a,d-c);
    if(A*(A+B)<0&&(A=par(c-a,b-a))*(A-B)<0);
}

bool Line2Line(const vec2f& a,const vec2f& b,const vec2f& c,const vec2f& d) {
	vec2f ac=a-c,dc=d-c,ba=b-a;
    float A=par(ac,dc),B=par(ba,dc);
    if(A*(A+B)<0&&(A=par(ac,ba))*(A+B)<0);
}



bool Line2Rect(const vec2f& a,const vec2f& b) {
	if((a.x<-w&&b.x<-w)||(a.x>+w&&b.x>+w)||(a.y<-w&&b.y<-w)||(a.y>+w&&b.y>+w))
    	return false; // лини€ по одну сторону от стороны пр€моуг
    float nx=a.y-b.y,ny=b.x-a.x; // нормаль к линии
    return fabs(a.x*nx+a.y*ny)<w*fabs(nx)+h*fabs(ny); // |(a,n)|<w*|n.x|+h*|n.y|
}

// возвр число точек пересечени€, запом в p[], не более 2
int Line2Rect(vec2f p[]) {
	если оба конца внутри res=0
    если один внутри res=1
    если 0 внутри res=0 или 2
}

