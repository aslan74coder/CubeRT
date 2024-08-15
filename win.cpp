#include "win.h"
#include <windows.h>
#include <stdio.h>
#include <stdexcept>

HWND  hwnd=0;
HDC   hdc=0;
int	  bpp=32; //???
int	  size=0;
void* buf=0;
MSG   msg;
int   keys[256]; // pressing duration since last check

class error : public std::runtime_error {
public:
    const char *file,*func;int line;
    error(const char* _msg,const char* _file,const char* _func,int _line) :
        std::runtime_error(_msg),file(_file),func(_func),line(_line) {};
};

//??? check compiler option "Merge duplicate strings" for one def of file and func
inline void Error(const char *msg) { throw error(msg,__FILE__,__FUNCTION__,__LINE__); }

void WinCheck(bool ok) {
	if(ok) return;
    static char s[256];
    DWORD err=GetLastError();
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,0,err,0,s,sizeof(s),0);
    Error(s);
}

int ResX() { return GetSystemMetrics(SM_CXSCREEN); }

int ResY() { return GetSystemMetrics(SM_CYSCREEN); }

int WinW() { RECT rect;GetClientRect(hwnd,&rect);return rect.right; }

int WinH() { RECT rect;GetClientRect(hwnd,&rect);return rect.bottom; }

bool WinActive() { return GetForegroundWindow()==hwnd; } // ??? всегда true

bool WinVisible() { return IsWindowVisible(hwnd); }

int GetMouse(int &x,int &y) {
	POINT p;
	GetCursorPos(&p);
    ScreenToClient(hwnd,&p);
    x=p.x;y=p.y;
    short sl=GetAsyncKeyState(VK_LBUTTON);
    short sm=GetAsyncKeyState(VK_MBUTTON);
    short sr=GetAsyncKeyState(VK_RBUTTON);
	return ((sl>>15)|(sm>>14)|(sr>>13)|(sl<<3)|(sm<<4)|(sr<<5))&0x7f;
}

void SetMouse(int x,int y) {
    POINT p;
    p.x=x;p.y=y;
    ClientToScreen(hwnd,&p);
    SetCursorPos(p.x,p.y);
}

//??? не работает
//??? менять/прятать только внутри окна
void SetCursorA(int i) {
/*???
	while(ShowCursor(1)<0); //??? показать
	while(ShowCursor(0)>=0);//??? спрятать
*/
	SetClassLong(hwnd,GCLP_HCURSOR,i?(int)GetCursor():-1);
}

int GetKey(char key) {
    int t=keys[key],t2=(t<0)?(Time()>>1):0;
    keys[key]=-t2;
    return t+t2;
}

unsigned int Time() { return GetTickCount(); } //??? take time in Active state only for correct physic sim

void Wait(unsigned int time) {
    if(time)
        Sleep(time);
    else
        WaitMessage();
}

bool Run() {
    if(!hwnd)
        return 0;
	while(PeekMessage(&msg,0,0,0,PM_REMOVE)) {
        if(msg.message==WM_QUIT)
            return 0;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
    };
    return hwnd;
}

void InitKeys() {
    int t=-Time()>>1;
    for(int key=0;key<256;++key)
        keys[key]=(GetAsyncKeyState(key)&0x8000)?t:0; // key down
}

void* WinData() {
    int w=WinW(),h=WinH(),size2=w*h*bpp;
	if(size2>size) {
		if(buf) free(buf);
		buf=malloc(size2);
		size=size2;
	}
	return buf;
}

void Flip() {
	int w=WinW(),h=WinH();
    BITMAPINFOHEADER bih;
    bih.biSize=sizeof(BITMAPINFOHEADER);
    bih.biWidth=w;
    bih.biHeight=h;
    bih.biPlanes=1;
    bih.biBitCount=bpp;
    bih.biCompression=BI_RGB;
    bih.biSizeImage=0;
    bih.biClrUsed=0;
    bih.biClrImportant=0;
    WinCheck(SetDIBitsToDevice(hdc,0,0,w,h,0,0,0,h,WinData(),(BITMAPINFO*)&bih,DIB_RGB_COLORS));
}

LRESULT __stdcall WndProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam) {
	//??? GetWindowLong(hwnd,GWL_USERDATA);
	switch(uMsg) {
    	/*???
        case WM_SETCURSOR :
		    if(LOWORD(lParam)==HTCLIENT) { // client area
            	DWORD hc=GetClassLong(hwnd,GCL_HCURSOR);
            	SetCursor(1?(HCURSOR)hc:0);
                return 0;
            } else
            	; // left as is
        	break;
        */

        /*???
        case WM_PAINT:
        	PAINTSTRUCT ps;
	        BeginPaint(hwnd,&ps);
            EndPaint(hwnd,&ps);
            //??? ValidateRect(hwnd,&ps.rcPaint);
            return 0;
            break;
        */

        case WM_ACTIVATE:
        	if(LOWORD(wParam)!=WA_INACTIVE) InitKeys();
    		break;
    	case WM_KEYDOWN :
    	    //??? получ время из структ сообщ-я
            if(keys[wParam]>=0) keys[wParam]-=(Time()>>1);
    		break;
	    case WM_KEYUP :
            if(keys[wParam]< 0) keys[wParam]+=(Time()>>1);
    		break;
        //??? case WM_DESTROY:
        case WM_CLOSE:
    		//PostQuitMessage(0);
            ::hwnd=0;
        	return 0;
	}
	return DefWindowProc(hwnd,uMsg,wParam,lParam);
}

void Window(char* title,int w,int h) {
    int fullScreen=!(w&&h),style;
    if(fullScreen) {
        style=WS_MAXIMIZE|WS_POPUP; // is this no-border ???
    }
    else {
        style=WS_OVERLAPPEDWINDOW;
        RECT rect={0,0};
        rect.right=w-1;rect.bottom=h-1;
        AdjustWindowRect(&rect,style,0);
        w=rect.right-rect.left+1;h=rect.bottom-rect.top+1;
    }

    if(!hwnd) {
		WNDCLASS wc;
		wc.hInstance=GetModuleHandle(NULL); //??? hThisInst
		wc.lpszClassName="WINAP";
		wc.lpfnWndProc=WndProc;
		wc.style=0;
		wc.hIcon=LoadIcon(NULL,IDI_APPLICATION); //??? wc.hInstance IDI_WINLOGO взять из EXE
		wc.hCursor=LoadCursor(NULL,IDC_ARROW);
		wc.lpszMenuName=NULL;
		wc.cbClsExtra=0;
		wc.cbWndExtra=0;
		wc.hbrBackground =(HBRUSH) COLOR_BACKGROUND;

		WinCheck(RegisterClass(&wc));
		WinCheck(hwnd=CreateWindow(wc.lpszClassName,title,style,CW_USEDEFAULT,CW_USEDEFAULT,w,h,0,0,wc.hInstance,0));
        WinCheck(hdc=GetDC(hwnd));
    }
    else {
	    SetWindowText(hwnd,title);
        SetWindowLong(hwnd,GWL_STYLE,style);
        SetWindowPos(hwnd,HWND_TOP,CW_USEDEFAULT,CW_USEDEFAULT,w,h,
                     SWP_SHOWWINDOW|SWP_NOMOVE|SWP_NOCOPYBITS|(fullScreen?SWP_NOSIZE:0));
    }
    ShowWindow(hwnd,fullScreen?SW_SHOWMAXIMIZED:SW_SHOWNORMAL);
	//??? UpdateWindow(hwnd);
}

inline bool isspc(char c) { return ((unsigned char)(c-1)<32); }; // is char 1-32

int split(char* s,char *a[]) {
    int n=0;
    while(*s) {
        a[n++]=s;
        while(*s&&!isspc(*s))
			++s;
        while(*s&&isspc(*s))
			*(s++)=0;
    }
    return n;
}

WINAPI WinMain(HINSTANCE hinst,HINSTANCE hprev,LPSTR cmd,int show) {
	cmd=GetCommandLine();
    char filename[MAX_PATH],*args[256]={filename};
    WinCheck(GetModuleFileName(0,filename,sizeof(filename)));
    int narg=split(cmd,args+1),res=0;
    try {
        res=wmain(narg+1,args);
    } catch(const error& e) {
        FILE* f=fopen("log.txt","wt");
        fputs(e.what(),f?f:stdout);
        fclose(f);
    }
//???    WinFree();
    return res;
};
