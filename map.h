#ifndef __mapH
#define __mapH

#define M 5
#define N (1<<M)

extern unsigned int rgb[256]; // palette
extern unsigned char map[N*N*N];

void InitMap();

inline int Color(unsigned int c) {
	unsigned int a=rgb[map[c>>2]];
	return a^(0x040404<<(c&3));
}

#endif
