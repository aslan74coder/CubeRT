#include "ray.h"

int Ray(const vec3f& p,const vec3f& l,float tm) {
	uint i,n=0,s[3],m=2*M; // приращения индексов по осям
	float t[3],d[3],pf,pi,di; // время достижения след коорд плоск и движения между ними
	for(i=0;i<3;++i) {
		pi=floor(pf=p[i]);
        //??? di=((di=l[i])!=0)?1./di:FLOAT_MAX;
        di=1./(di=l[i]);
		t[i]=(pi-pf+(di>0))*di;
		d[i]=fabs(di);
        s[i]=((sgn(di)+N)&(N-1))<<m;m-=M;
        (n<<=M)|=int(pi+N)&(N-1);
	}
    do {
        i=(t[2]<t[1])+1;i&=-(t[i]<t[0]);
    } while(!map[(n+=s[i])&=(N*N*N-1)]&&(t[i]+=d[i])<=tm);
    return (n<<2)|i;
}
