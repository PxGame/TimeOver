# include <Windows.h>
# include <iostream>
# include <string>
# include <WinBase.h>
using namespace std;

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
//设置字体
void SetFont(HWND hWnd, LPSTR FontName, int Size, int Weight);
//操作系统控制
void OperateSystemControl(UINT n);
//通过TimeOverControlInfo.txt文件设置参数
void SystemControlFile();
//设置开机启动
void SetWinStart (UINT n);

//窗口类名
LPSTR lpClassName = "Window";

//消息循环
MSG msg;

//获取屏幕大小,
UINT SCW = ::GetSystemMetrics(SM_CXSCREEN);
UINT SCH = ::GetSystemMetrics(SM_CYSCREEN);

//设置窗口大小
UINT WinW = SCW;
UINT WinH = SCH;

//获取窗口用户区
RECT rtUser;                    //用户区坐标
int wUser;                      //宽
int hUser;                      //高
HINSTANCE hInst;                //句柄实例
HWND hWnd;                      //主窗口句柄
HWND hButton_1;                 //按钮1句柄
HWND Static_1;                  //静态文本1句柄

POINT pControl;                 //控件坐标

//计时器设置
UINT uTime_Space = 7200;           //设置默认间隔时间，单位秒
UINT uTime_Residue = 120;         //设置默认倒计时时间，单位秒
UINT uTime_Residue_Run = 0;

//设置OperateSystemControl
UINT nOSC = 5;//模式模式
UINT nOSO = 2;//设置开机启动

//程序完整路径
char filePath[256]= {};

//文本格式化用
char strText[20] = {};

//文件指针
FILE *file;

int WINAPI WinMain (HINSTANCE hInstance,    
    HINSTANCE hPrevInstance,                
    LPSTR nCmdLine,                         
    int nCmdShow)                          
{
    //复制
    ::hInst = hInstance;

    //通过TimeOverControlInfo.txt文件设置参数
    SystemControlFile();
    //设置是否开机启动
    SetWinStart (nOSO);
    //初始化
    InitializeWindow(hInstance);

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
        ::SetTimer(hWnd, 1, uTime_Space*1000, TimerProc);//窗口初始化中的ShowWindow不能引起WM_SHOWWINDOW，所以在这设置间隔提醒时钟
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
        if (wParam == TRUE)
        {
            uTime_Residue_Run = uTime_Residue;//初始化倒计时

            sprintf (strText, "%d 秒后自动关机，请单击解除。", uTime_Residue_Run);
            ::SetWindowText(::hButton_1, strText);//初始化按钮文本

            ::KillTimer(hWnd, 1);//关闭间隔时钟
            ::SetTimer(::hWnd, 0, 1000, TimerProc);//倒计时时钟设置为1秒
        }
        else
        {
            ::SetTimer(hWnd, 1, uTime_Space*1000, TimerProc);//间隔提醒时钟设置为uTime_Space秒
            ::KillTimer(hWnd, 0);//关闭倒计时时钟
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
        ::uTime_Residue_Run--;
        sprintf (strText, "%d 秒后自动关机，请单击解除。", uTime_Residue_Run);
        ::SetWindowText(::hButton_1, strText);
        if (uTime_Residue_Run == 0)//倒计时结束，执行关机或其他指令
        {
            ::KillTimer(::hWnd, 0);
            OperateSystemControl(nOSC);
        }
        break;
    case 1:      
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
    ::hWnd = CreateWindow(::lpClassName, "提示",  /*WS_SYSMENU |*/ WS_THICKFRAME | BS_MULTILINE, 
        (::SCW - ::WinW)/2, (::SCH - ::WinH)/2, ::WinW, ::WinH, 
        NULL, NULL, hInstance, NULL);
    
    //获取用户区坐标及大小
    ::GetClientRect(::hWnd, &::rtUser);
    ::wUser = ::rtUser.right - ::rtUser.left;
    ::hUser = ::rtUser.bottom - ::rtUser.top;
    
    //初始化控件
    InitializeControl();

    //显示窗口
    ::ShowWindow(::hWnd, SW_HIDE);
    ::UpdateWindow(::hWnd);
    
    //消息循环
    while(GetMessage(&msg,NULL,0,0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}     
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
    hButton_1 = CreateWindow("Button", "单击取消关机", BS_DEFPUSHBUTTON | WS_CHILD | WS_VISIBLE | BS_CENTER, 
        ::pControl.x, ::pControl.y, ::wUser, ::hUser, ::hWnd, (HMENU)btnID_1, hInst, NULL);
    SetFont(hButton_1, "楷体", ::wUser/16, 200);//hButton_1设置字体
}

void UpdateControlSize()
{
    //更改按钮大小
    ::MoveWindow(::hButton_1, 0, 0, ::wUser, ::hUser, false);
    //更改控件中的字体大小
    SetFont(hButton_1, "楷体", ::wUser/16, 200);
}

//设置字体
void SetFont(HWND hWnd, LPSTR FontName, int Size, int Weight)
{
    //HDC hdc = GetDC(hWnd);

    LOGFONT LogFont;
    ::memset(&LogFont, 0, sizeof(LOGFONT));
    lstrcpy(LogFont.lfFaceName, FontName);
    LogFont.lfWeight = Weight;
    LogFont.lfHeight = Size; // 字体大小
    LogFont.lfCharSet = 134;
    LogFont.lfOutPrecision = 3;
    LogFont.lfClipPrecision = 2;
    LogFont.lfOrientation = 45;
    LogFont.lfQuality = 1;
    LogFont.lfPitchAndFamily = 2;

    // 创建字体

    HFONT hFont = CreateFontIndirect(&LogFont);

    //LPTEXTMETRIC TM;//获取字体信息
    //::GetTextMetrics(hdc, TM);

    //设置背景

    // 设置控件字体
    ::SendMessage(hWnd, WM_SETFONT, (WPARAM)hFont, 0); 

    //::ReleaseDC(hWnd, hdc);
}

void OperateSystemControl(UINT n)
{
    //-------------------------------调整进程权限
    HANDLE handle;//进程访问令牌的句柄
    TOKEN_PRIVILEGES tkp; //特权令牌
    int result;//是否正确执行

    if (!::OpenProcessToken(::GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &handle))//获得进程访问令牌的句柄
    {
        MessageBox(NULL, "OpenProcessToken failed!", "错误", NULL);   //获得进程句柄失败
        ::PostQuitMessage(0);
    }
    ::LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME ,&tkp.Privileges[0].Luid);//获得本地机唯一的标识
    tkp.PrivilegeCount = 1;//只有一个标识
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;//打开Luid对应的特权
    result = AdjustTokenPrivileges(handle, FALSE, &tkp, 0, NULL, 0);   //调整获得的权限
    if (result != TRUE)
    {
        MessageBox(NULL, "AdjustTokenPrivileges enable failed!", "错误", NULL);  //修改权限失败
        ::PostQuitMessage(0);
    }
    //---------------------------------------------------------

    switch (n)
    {
    case 0://正常关机
        result = ::InitiateSystemShutdown(NULL ,NULL, 0, FALSE, FALSE);//ExitWindowsEx(EWX_SHUTDOWNF,0);
        break;
    case 1://强制关机
        result = ::InitiateSystemShutdown(NULL ,NULL, 0, TRUE, FALSE);//ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE,0);
        break;
    case 2://重启
        result = ::InitiateSystemShutdown(NULL ,NULL, 0, FALSE, TRUE);//ExitWindowsEx(EWX_REBOOT,0);
        break;
    case 3://强制重启
        result = ::InitiateSystemShutdown(NULL ,NULL, 0, TRUE, TRUE);//ExitWindowsEx(EWX_REBOOT | EWX_FORCE,0);
        break;
    case 4://销注
        result = ExitWindowsEx(EWX_LOGOFF,0);
        break;
    case 5://锁定用户
        result = ::LockWorkStation();
        break;
    }
    if (result != TRUE)
    {
        MessageBox(NULL, "OperateSystemControl enable failed!", "错误", NULL);//系统控制失败
    }
}

void SystemControlFile()
{
    char ch[100] = {};
    file = fopen("TimeOverControlInfo.txt", "r+");
    if (!file)
    {
        file = fopen("TimeOverControlInfo.txt", "a");
        fprintf (file, "%d+%d+%d+%d\n设置格式为：间隔提醒时间+倒计时时间+模式+开机是否启动\n单位均为秒,且只能在文件开头处顶格输入，用‘+’分开，中间不能有空格，不能为其他内容。\n模式: 0，正常关机   1，强制关机   2，重启   3，强制重启   4，销注   5，锁定用户\n是否开机启动： 0，开机不启动  1，开机启动   2，默认设置（不启动）", 
            uTime_Space, uTime_Residue, nOSC, nOSO);
        fclose (file);
        file = fopen("TimeOverControlInfo.txt", "r+");
    }
    fscanf (file, "%d+%d+%d+%d", &uTime_Space, &uTime_Residue, &nOSC, &nOSO);
}

void SetWinStart (UINT n)
{
    HKEY hKey;
    int result = 0;
    ::GetModuleFileName(hInst, filePath, 256);//获取程序完整路径

    ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_ALL_ACCESS , &hKey);

    switch (n)
    {
    case 0://取消开机启动
        result = ::RegDeleteValue(hKey, "TimeOver");
        break;
    case 1://设置开机启动
        result = ::RegSetValueEx(hKey, "TimeOver", 0, REG_SZ, (BYTE*)filePath, 256);//添加注册表
        break;
    case 2://不做任何设置
        result = 0;
        break;
    }

    ::RegCloseKey(hKey);

    if (result)//判断是否成功
    {
        MessageBox(NULL, "SetWinStart enable fail!", "错误", NULL);
    }
}

void BubbleMsg(char *ch)
{
    MessageBox(NULL, ch, "提示", NULL);
}
