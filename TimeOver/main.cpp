# include <Windows.h>
# include <iostream>

# define btnID_1 1000 //定义按钮1的资源ID

//消息循环
LRESULT CALLBACK WinProc (HWND hWnd, UINT uMsg, LPARAM lParam, WPARAM wParam);
//时间消息接受函数
void CALLBACK TimerProc (HWND hWnd, UINT uMsg, UINT nIDEvent, DWORD dwTime);                           
//初始化窗口
void InitializeWindow(HINSTANCE hInstance);
//初始化控件
void InitializeControl();
//更新控件大小
void UpdateControlSize();

LPSTR lpClassName = "Window";

//获取屏幕大小,
UINT SCW = ::GetSystemMetrics(SM_CXSCREEN);
UINT SCH = ::GetSystemMetrics(SM_CYSCREEN);

//设置窗口大小
UINT WinW = 500;
UINT WinH = 300;

//获取窗口用户区
POINT pControl;//控件坐标
RECT rtUser;//用户区坐标
int wUser;//宽
int hUser;//高
HINSTANCE hInst;//句柄实例
HWND hWnd;//主窗口句柄
HWND hButton_1;//按钮1句柄
HWND Static_1;//静态文本1句柄

char strText[20];
UINT uTime_Space = 0;//设置间隔时间
UINT uTime_Residue = 10;//设置倒计时时间
UINT uTime_Residue_Run = 0;



int WINAPI WinMain (HINSTANCE hInstance,    
    HINSTANCE hPrevInstance,                
    LPSTR nCmdLine,                         
    int nCmdShow)                          
{
    //复制
    ::hInst = hInstance;

    //初始化窗口
    InitializeWindow(hInstance);

    //消息循环
    MSG msg;
    while(GetMessage(&msg,NULL,0,0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;

}
LRESULT CALLBACK WinProc (HWND hWnd,        
    UINT uMsg,                              
    WPARAM wParam,                          
    LPARAM lParam)                          
{
    int wmID;
    int wmEvent;
    switch (uMsg)
    {
    case WM_CREATE :  
        break;
    case WM_PAINT :        
        break;
    case WM_COMMAND :
        wmID = LOWORD(wParam);
        wmEvent = HIWORD(wParam);
        switch (wmID)
        {
        case btnID_1 :
            ::ShowWindow(hWnd, SW_HIDE);
            break;
        }
        break;
    case WM_SHOWWINDOW :
        if (wParam == true)
        {
            ::KillTimer(hWnd, 1);//关闭间隔时钟
            ::SetTimer(::hWnd, 0, 1000, TimerProc);//倒计时时钟设置
        }
        else
        {
            ::KillTimer(hWnd, 0);//关闭倒计时时钟
            ::SetTimer(hWnd, 1, 10000, TimerProc);//间隔提醒时钟设置
        }
        break;
    case WM_SIZE :
        ::hUser = HIWORD(lParam);
        ::wUser = LOWORD(lParam);
        UpdateControlSize();
        break;
    case WM_TIMER :
        break;
    case WM_CLOSE :      
        if (IDYES == MessageBox(::hWnd,"是否关闭窗口？","提示",MB_YESNO))
        {
            ::DestroyWindow(hWnd);
        }
        break;
    case WM_DESTROY :
        ::PostQuitMessage(0);
        break;
    }
    return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}
void CALLBACK TimerProc (HWND hWnd,         
    UINT uMsg,                              
    UINT nIDEvent,                          
    DWORD dwTime)
{
    switch (nIDEvent)
    {
    case 0:
        uTime_Residue_Run--;
        sprintf (strText, "%d 秒后自动关机，请单击解除。", uTime_Residue_Run);
        ::SetWindowText(::hButton_1, strText);
        if (uTime_Residue_Run == 0)
        {
            ::KillTimer(::hWnd, 0);
            MessageBox(NULL, "TimeOver!","！", MB_OK);
        }
        break;
    case 1:        
        uTime_Residue_Run = uTime_Residue;//初始化倒计时
        ::ShowWindow(hWnd, SW_SHOW);
        break;
    }
}
void InitializeWindow(HINSTANCE hInstance)
{
    //创建窗口类
    WNDCLASS wnd;
    wnd.cbClsExtra          = 0;
    wnd.cbWndExtra          = 0;
    wnd.hbrBackground       = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wnd.hCursor             = LoadCursor(NULL, IDC_ARROW);
    wnd.hIcon               = LoadIcon(NULL, IDI_WINLOGO);
    wnd.hInstance           = hInstance;
    wnd.lpfnWndProc         = ::WinProc;
    wnd.lpszClassName       = ::lpClassName;
    wnd.lpszMenuName        = NULL;
    wnd.style               = CS_HREDRAW | CS_VREDRAW; //在窗口宽度或高度发生变化是引发重绘
    
    //注册窗口类
    ::RegisterClass(&wnd);
    
    //创建窗口,居中显示
    ::hWnd = CreateWindow(::lpClassName, "提示", WS_OVERLAPPED | WS_SYSMENU | WS_THICKFRAME, 
        (::SCW - ::WinW)/2, (::SCH - ::WinH)/2, ::WinW, ::WinH, 
        NULL, NULL, hInstance, NULL);
    
    //获取用户区坐标及大小
    ::GetClientRect(::hWnd, &::rtUser);
    ::wUser = ::rtUser.right - ::rtUser.left;
    ::hUser = ::rtUser.bottom - ::rtUser.top;
    
    //初始化控件
    InitializeControl();

    //显示窗口
    ::ShowWindow(::hWnd, SW_SHOWNORMAL);
    ::UpdateWindow(::hWnd);
     
}
void InitializeControl()
{
    //设置控件坐标
    ::pControl.x = 0;
    ::pControl.y = 0;

    //ClientToScreen(::hWnd, &pControl);//不用转换
    //char ch[100];
    //sprintf (ch, "lx=%ld,ly=%ld\n%ld,%ld\nw=%d,h=%d",
    //    ::pControl.x,::pControl.y,SCW,SCH,wUser,hUser);
    //MessageBox (hWnd, ch, "检测数据",MB_OK);
    
    //添加控件
    hButton_1 = CreateWindow("Button", "单击取消关机", BS_DEFPUSHBUTTON | WS_CHILD | WS_VISIBLE, 
        ::pControl.x, ::pControl.y, ::wUser, ::hUser, ::hWnd, (HMENU)btnID_1, hInst, NULL);
}
void UpdateControlSize()
{
    //更改按钮大小
    ::MoveWindow(::hButton_1, 0, 0, ::wUser, ::hUser, false);
}
