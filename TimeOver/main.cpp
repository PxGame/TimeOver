# include <Windows.h>
# include <iostream>
# include <string>
# include <WinBase.h>
using namespace std;

# define btnID_1 1000 //���尴ť1����ԴID

//��Ϣѭ��
LRESULT CALLBACK WinProc (HWND hWnd, UINT uMsg, LPARAM lParam, WPARAM wParam);
//ʱ����Ϣ���ܺ���
void CALLBACK TimerProc (HWND hWnd, UINT uMsg, UINT nIDEvent, DWORD dwTime);                           
//��ʼ������
void InitializeWindow(HINSTANCE hInstance);
//��ʼ���ؼ�
void InitializeControl();
//���¿ؼ���С
void UpdateControlSize();
//��������
void SetFont(HWND hWnd, LPSTR FontName, int Size, int Weight);
//����ϵͳ����
void OperateSystemControl(UINT n);
//ͨ��TimeOverControlInfo.txt�ļ����ò���
void SystemControlFile();
//���ÿ�������
void SetWinStart (UINT n);

//��������
LPSTR lpClassName = "Window";

//��Ϣѭ��
MSG msg;

//��ȡ��Ļ��С,
UINT SCW = ::GetSystemMetrics(SM_CXSCREEN);
UINT SCH = ::GetSystemMetrics(SM_CYSCREEN);

//���ô��ڴ�С
UINT WinW = SCW;
UINT WinH = SCH;

//��ȡ�����û���
RECT rtUser;                    //�û�������
int wUser;                      //��
int hUser;                      //��
HINSTANCE hInst;                //���ʵ��
HWND hWnd;                      //�����ھ��
HWND hButton_1;                 //��ť1���
HWND Static_1;                  //��̬�ı�1���

POINT pControl;                 //�ؼ�����

//��ʱ������
UINT uTime_Space = 7200;           //����Ĭ�ϼ��ʱ�䣬��λ��
UINT uTime_Residue = 120;         //����Ĭ�ϵ���ʱʱ�䣬��λ��
UINT uTime_Residue_Run = 0;

//����OperateSystemControl
UINT nOSC = 5;//ģʽģʽ
UINT nOSO = 2;//���ÿ�������

//��������·��
char filePath[256]= {};

//�ı���ʽ����
char strText[20] = {};

//�ļ�ָ��
FILE *file;

int WINAPI WinMain (HINSTANCE hInstance,    
    HINSTANCE hPrevInstance,                
    LPSTR nCmdLine,                         
    int nCmdShow)                          
{
    //����
    ::hInst = hInstance;

    //ͨ��TimeOverControlInfo.txt�ļ����ò���
    SystemControlFile();
    //�����Ƿ񿪻�����
    SetWinStart (nOSO);
    //��ʼ��
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
        ::SetTimer(hWnd, 1, uTime_Space*1000, TimerProc);//���ڳ�ʼ���е�ShowWindow��������WM_SHOWWINDOW�������������ü������ʱ��
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
            uTime_Residue_Run = uTime_Residue;//��ʼ������ʱ

            sprintf (strText, "%d ����Զ��ػ����뵥�������", uTime_Residue_Run);
            ::SetWindowText(::hButton_1, strText);//��ʼ����ť�ı�

            ::KillTimer(hWnd, 1);//�رռ��ʱ��
            ::SetTimer(::hWnd, 0, 1000, TimerProc);//����ʱʱ������Ϊ1��
        }
        else
        {
            ::SetTimer(hWnd, 1, uTime_Space*1000, TimerProc);//�������ʱ������ΪuTime_Space��
            ::KillTimer(hWnd, 0);//�رյ���ʱʱ��
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
        if (IDYES == MessageBox(::hWnd,"�Ƿ�رմ��ڣ�","��ʾ",MB_YESNO))
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
        sprintf (strText, "%d ����Զ��ػ����뵥�������", uTime_Residue_Run);
        ::SetWindowText(::hButton_1, strText);
        if (uTime_Residue_Run == 0)//����ʱ������ִ�йػ�������ָ��
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
    //����������
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
    wnd.style               = CS_HREDRAW | CS_VREDRAW; //�ڴ��ڿ�Ȼ�߶ȷ����仯�������ػ�
    
    //ע�ᴰ����
    ::RegisterClass(&wnd);
    
    //��������,������ʾ
    ::hWnd = CreateWindow(::lpClassName, "��ʾ",  /*WS_SYSMENU |*/ WS_THICKFRAME | BS_MULTILINE, 
        (::SCW - ::WinW)/2, (::SCH - ::WinH)/2, ::WinW, ::WinH, 
        NULL, NULL, hInstance, NULL);
    
    //��ȡ�û������꼰��С
    ::GetClientRect(::hWnd, &::rtUser);
    ::wUser = ::rtUser.right - ::rtUser.left;
    ::hUser = ::rtUser.bottom - ::rtUser.top;
    
    //��ʼ���ؼ�
    InitializeControl();

    //��ʾ����
    ::ShowWindow(::hWnd, SW_HIDE);
    ::UpdateWindow(::hWnd);
    
    //��Ϣѭ��
    while(GetMessage(&msg,NULL,0,0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}     
}

void InitializeControl()
{
    //���ÿؼ�����
    ::pControl.x = 0;
    ::pControl.y = 0;

    //ClientToScreen(::hWnd, &pControl);//����ת��
    //char ch[100];
    //sprintf (ch, "lx=%ld,ly=%ld\n%ld,%ld\nw=%d,h=%d",
    //    ::pControl.x,::pControl.y,SCW,SCH,wUser,hUser);
    //MessageBox (hWnd, ch, "�������",MB_OK);
    
    //��ӿؼ�
    hButton_1 = CreateWindow("Button", "����ȡ���ػ�", BS_DEFPUSHBUTTON | WS_CHILD | WS_VISIBLE | BS_CENTER, 
        ::pControl.x, ::pControl.y, ::wUser, ::hUser, ::hWnd, (HMENU)btnID_1, hInst, NULL);
    SetFont(hButton_1, "����", ::wUser/16, 200);//hButton_1��������
}

void UpdateControlSize()
{
    //���İ�ť��С
    ::MoveWindow(::hButton_1, 0, 0, ::wUser, ::hUser, false);
    //���Ŀؼ��е������С
    SetFont(hButton_1, "����", ::wUser/16, 200);
}

//��������
void SetFont(HWND hWnd, LPSTR FontName, int Size, int Weight)
{
    //HDC hdc = GetDC(hWnd);

    LOGFONT LogFont;
    ::memset(&LogFont, 0, sizeof(LOGFONT));
    lstrcpy(LogFont.lfFaceName, FontName);
    LogFont.lfWeight = Weight;
    LogFont.lfHeight = Size; // �����С
    LogFont.lfCharSet = 134;
    LogFont.lfOutPrecision = 3;
    LogFont.lfClipPrecision = 2;
    LogFont.lfOrientation = 45;
    LogFont.lfQuality = 1;
    LogFont.lfPitchAndFamily = 2;

    // ��������

    HFONT hFont = CreateFontIndirect(&LogFont);

    //LPTEXTMETRIC TM;//��ȡ������Ϣ
    //::GetTextMetrics(hdc, TM);

    //���ñ���

    // ���ÿؼ�����
    ::SendMessage(hWnd, WM_SETFONT, (WPARAM)hFont, 0); 

    //::ReleaseDC(hWnd, hdc);
}

void OperateSystemControl(UINT n)
{
    //-------------------------------��������Ȩ��
    HANDLE handle;//���̷������Ƶľ��
    TOKEN_PRIVILEGES tkp; //��Ȩ����
    int result;//�Ƿ���ȷִ��

    if (!::OpenProcessToken(::GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &handle))//��ý��̷������Ƶľ��
    {
        MessageBox(NULL, "OpenProcessToken failed!", "����", NULL);   //��ý��̾��ʧ��
        ::PostQuitMessage(0);
    }
    ::LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME ,&tkp.Privileges[0].Luid);//��ñ��ػ�Ψһ�ı�ʶ
    tkp.PrivilegeCount = 1;//ֻ��һ����ʶ
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;//��Luid��Ӧ����Ȩ
    result = AdjustTokenPrivileges(handle, FALSE, &tkp, 0, NULL, 0);   //������õ�Ȩ��
    if (result != TRUE)
    {
        MessageBox(NULL, "AdjustTokenPrivileges enable failed!", "����", NULL);  //�޸�Ȩ��ʧ��
        ::PostQuitMessage(0);
    }
    //---------------------------------------------------------

    switch (n)
    {
    case 0://�����ػ�
        result = ::InitiateSystemShutdown(NULL ,NULL, 0, FALSE, FALSE);//ExitWindowsEx(EWX_SHUTDOWNF,0);
        break;
    case 1://ǿ�ƹػ�
        result = ::InitiateSystemShutdown(NULL ,NULL, 0, TRUE, FALSE);//ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE,0);
        break;
    case 2://����
        result = ::InitiateSystemShutdown(NULL ,NULL, 0, FALSE, TRUE);//ExitWindowsEx(EWX_REBOOT,0);
        break;
    case 3://ǿ������
        result = ::InitiateSystemShutdown(NULL ,NULL, 0, TRUE, TRUE);//ExitWindowsEx(EWX_REBOOT | EWX_FORCE,0);
        break;
    case 4://��ע
        result = ExitWindowsEx(EWX_LOGOFF,0);
        break;
    case 5://�����û�
        result = ::LockWorkStation();
        break;
    }
    if (result != TRUE)
    {
        MessageBox(NULL, "OperateSystemControl enable failed!", "����", NULL);//ϵͳ����ʧ��
    }
}

void SystemControlFile()
{
    char ch[100] = {};
    file = fopen("TimeOverControlInfo.txt", "r+");
    if (!file)
    {
        file = fopen("TimeOverControlInfo.txt", "a");
        fprintf (file, "%d+%d+%d+%d\n���ø�ʽΪ���������ʱ��+����ʱʱ��+ģʽ+�����Ƿ�����\n��λ��Ϊ��,��ֻ�����ļ���ͷ���������룬�á�+���ֿ����м䲻���пո񣬲���Ϊ�������ݡ�\nģʽ: 0�������ػ�   1��ǿ�ƹػ�   2������   3��ǿ������   4����ע   5�������û�\n�Ƿ񿪻������� 0������������  1����������   2��Ĭ�����ã���������", 
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
    ::GetModuleFileName(hInst, filePath, 256);//��ȡ��������·��

    ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_ALL_ACCESS , &hKey);

    switch (n)
    {
    case 0://ȡ����������
        result = ::RegDeleteValue(hKey, "TimeOver");
        break;
    case 1://���ÿ�������
        result = ::RegSetValueEx(hKey, "TimeOver", 0, REG_SZ, (BYTE*)filePath, 256);//���ע���
        break;
    case 2://�����κ�����
        result = 0;
        break;
    }

    ::RegCloseKey(hKey);

    if (result)//�ж��Ƿ�ɹ�
    {
        MessageBox(NULL, "SetWinStart enable fail!", "����", NULL);
    }
}

void BubbleMsg(char *ch)
{
    MessageBox(NULL, ch, "��ʾ", NULL);
}
