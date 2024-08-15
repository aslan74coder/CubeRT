/*???
�������� �� ������� (������� �� ������) �����. ��� ������ ���� ������������ ������ ������

����� �������� (������ 2 ����� � Ray), ���������, �����
������������ ����� �������, ������ �����
����� ����� ����� ��������������

����� ������������ ��� 6 ������ skybox, ����� ������� �� �� �����
�������� ������ AA �������� - ������� ������� � ����� � ���������
��� ������ ������ skybox -
����� ����� ��� �������������� � ��������� � ������������-���������
� ����������� ������ �� ���������� ������� ������
����� ���������

��� �������� ����� ��� ����� GL

��� ����� �������� ������� ������� � GL
������� = 4 �������,
�� ���������� �������� ����� ������, 1 byte �� �����
� ��� ������� - 1 ����, �� ��� 2 ��� - ���������� �������
���� ����� ���-�� � ������� ����� gl_VertexId%4
��������� ������ �� 4 byte �� ������� ���� � ����� ����� instancing

---------------------------------------------------------------------

����� ��� 4 ���� in - ������ in �� ���������, DrawQuad2(), SRay2()

����� ������ �-��� �� ����

���� ������ � ������� - ����� skybox ��� ����� ����� ����� ����� GL - ������� p,
l ������ ��� �������� in

---------------------------------------------------------------------

mipmap ��� map
������������ ��� ��� ����� ������ 16x16x16 (4�b)
� ������� ������ ������ �������� �����
� ��� ���� mipmap 8x8x8, 4x4x4, 2x2x2, 1x1x1
����������� -
1 ����
2 ���� � lod x2
4 ���� � lod x4
8 ���� � lod x8
16 ���� � lod x16

���� 8 ��� ����� - ��� x2 - ����
� mip ����� ������ ������ ���� - ��� ��������� ���������� ����
������ ������� ���� - ���� ���������, ����� � ����������� ���� ���� ����������?

��, ��� ���� ������� �� �� ���������� � LOD level
���� ��������� max ������ ���
��� ������� � ���� (���� ����� ������ ���� - �� ��� � ����� �����)
���� ���� ���� (lod x16) ���� - ����������
����� �� lod x8 ���
t,d - ��� ���� ������� 1
��� lod x s (s=2^n): d*s, t+d*(s-1)
d*2*s, t+d*(2*s-1)

*/

#include <ata/win.h>
#include <ata/vec.h>
#include <stdlib.h>
#include <locale.h>
#include "draw.h"
#include "map.h"

int w=800,h=600,dist=32,dens=20,stat=0;
float velo=1e-5;
float zn=0.1,zf=10,fov=90;
mat34f mat(mat3f(),vec3f(N/2,N/2,N/2));
extern int maxlev;

void MouseMove() {
	int cx=WinW()/2,cy=WinH()/2,x,y;
    GetMouse(x,y);
	mat.rot.ez()=(mat.rot.ez()+float(x-cx)/cx*mat.rot.ex()+float(cy-y)/cx*mat.rot.ey()).unit();
    mat.rot.ex()=cross(mat.rot.ey(),mat.rot.ez()).unit();
	mat.rot.ey()=cross(mat.rot.ez(),mat.rot.ex());
	SetMouse(cx,cy);
}

void KeyPress(float dt) {
    if(GetKey(27)>0) exit(0); //??? VK_ESCAPE
    //??? if(GetKey(VK_F2)) Screenshot();
    //??? ��������� unsigned - ����� �� ������ ��� �������� �����
	mat.rot=rot(vec3f(0.f,0.f,1.f),float(1e-5*dt*(GetKey('D')-GetKey('A'))))*mat.rot;
	mat.pos+=velo*dt*(GetKey('S')-GetKey('W'))*mat.rot.ez();
}

char* strbeg(char* s,char* beg) {
	for(char c;c=*beg;++s,++beg) {
		if(*s!=c)
			return 0;
	}
	return s;
}

void ParseCommandLine(int narg,char *args[]) {
	for(int i=1;i<narg;++i) {
		char *s=args[i],*t;
		if(t=strbeg(s,"res=")) {
			w=atoi(t);
			while(*t>32&&*t!='x') ++t;
			if(*t)
				h=atoi(t+1);
		}
		else
		if(t=strbeg(s,"dist=")) {
			dist=atoi(t);
		}
		else
		if(t=strbeg(s,"dens=")) {
			dens=atoi(t);
		}
		else
		if(t=strbeg(s,"lev=")) {
			maxlev=atoi(t);
		}
		else
		if(t=strbeg(s,"stat=")) {
			stat=atoi(t);
		}
	}
}

int main(int narg,char *args[]) {
	ParseCommandLine(narg,args);
    InitMap();
    Window("Cubes Ray Tracing",w,h);
    //??? SetCursorA(0);
    SetMouse(w/2,h/2);
	for(int t=Time(),u; Run(); Wait(20)) {
        if(!WinActive()) continue;
        MouseMove();
        u=t;t=Time();KeyPress(t-u);
        Draw(mat,w,h,fov,1,dist,(int*)WinData()); //??? dist=zf/zn
        Flip();
        if(stat) {

			setlocale(LC_NUMERIC,"");
			struct lconv *ptrLocale = localeconv();
			ptrLocale->decimal_point=".";
			ptrLocale->thousands_sep=" ";
			sprintf(s,"POS %f3.2 %f3.2 %f3.2",mat.pos.x,mat.pos.y,mat.pos.z);
			TextOut(hdc,x,y,s,strlen(s));
			sprintf(s,"FPS %d3",fps);
			TextOut(hdc,x,y,s,strlen(s));
			sprintf(s,"RAYS %'d7",nrays);
			TextOut(hdc,x,y,s,strlen(s));
        }
	}
    return 0;
};
