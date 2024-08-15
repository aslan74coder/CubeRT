#include "map.h"
#include <stdlib.h>

typedef unsigned int uint;

extern int dens;

unsigned int rgb[256];
unsigned char map[N*N*N]; //??? mip-map

void InitMap() {
	rgb[0]=0;
	for(int i=1;i<256;++i) {
		uint r=rand()&0xff;
		uint g=rand()&0xff;
		uint b=rand()&0xff;
		rgb[i]=b|(g<<8)|(r<<16);
	}
    for(int i=0;i<N*N*N;++i) {
		uint fill=((rand()&127)<dens);
		map[i]=rand()&255&-fill;
    }
};
