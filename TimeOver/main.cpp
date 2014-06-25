# include <Windows.h>
# include <iostream>

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

//��������
LPSTR lpClassName = "Window";

//��ȡ��Ļ��С,
UINT SCW = ::GetSystemMetrics(SM_CXSCREEN);
UINT SCH = ::GetSystemMetrics(SM_CYSCREEN);

//���ô��ڴ�С
UINT WinW = SCW;
UINT WinH = SCH;

//��ȡ�����û���
POINT pControl;//�ؼ�����
RECT rtUser;//�û�������
int wUser;//��
int hUser;//��
HINSTANCE hInst;//���ʵ��
HWND hWnd;//�����ھ��
HWND hButton_1;//��ť1���
HWND Static_1;//��̬�ı�1���

char strText[20];
UINT uTime_Space = 5;//���ü��ʱ�䣬��λ��
UINT uTime_Residue = 10;//���õ���ʱʱ�䣬��λ��
UINT uTime_Residue_Run = 0;



int WINAPI WinMain (HINSTANCE hInstance,    
    HINSTANCE hPrevInstance,                
    LPSTR nCmdLine,                         
    int nCmdShow)                          
{
    //����
    ::hInst = hInstance;

    //��ʼ������
    InitializeWindow(hInstance);

    //��Ϣѭ��
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
        if (wParam == true)
        {
            uTime_Residue_Run = uTime_Residue;//��ʼ������ʱ
            ::KillTimer(hWnd, 1);//�رռ��ʱ��
            ::SetTimer(::hWnd, 0, 1000, TimerProc);//����ʱʱ������1s
        }
        else
        {
            ::SetTimer(hWnd, 1, uTime_Space*1000, TimerProc);//�������ʱ������
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
        if (uTime_Residue_Run == 0)
        {
            ::KillTimer(::hWnd, 0);
            MessageBox(NULL, "TimeOver!","��", MB_OK);
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
    ::hWnd = CreateWindow(::lpClassName, "��ʾ",  WS_SYSMENU | WS_THICKFRAME | BS_MULTILINE, 
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
    SetFont(hButton_1, "����", ::wUser/16, 200);
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
