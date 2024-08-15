#ifndef __winH
#define __winH
//??? #include "defs.h"

#define main wmain

int ResW();
int ResH();
int Bpp();

// Window

void  Window(char* title,int w=0,int h=0); // w/h=0 - full screen
int	  WinW();
int	  WinH();
void* WinData();
bool  WinActive();
bool  WinVisible();
void  Flip(); // flip window buf

// Keyboard

int   GetKey(char key); // duration of pressing since last check, ms

// Mouse

int   GetMouse(int &x,int &y); // returns L,M,R down in bits 0,1,2,
// L,M,R pressed since previous call - bits 3,4,5, x,y - window coords
void  SetMouse(int x,int y);   // move cursor to x,y
void  SetCursorA(int);         // set cursor type, 0 - none

// Application

unsigned int  Time();         // time since start, ms
void  Wait(unsigned int time=0);// wait time ms or first message if 0
bool  Run();          // process messages, return TRUE while app is running

#if defined __WIN32__ && !defined __CONSOLE__
int main(int narg,char *args[]);
#endif

#endif
