// GUISendFile(server).cpp : ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#include<Commdlg.h>
#include<CommCtrl.h>
#include "GUISendFile(server).h"
#include<thread>
#include "threadFunc.h"
#define MAX_LOADSTRING 100
#define ID_BUTTON 10000
#define IDC_IPADDR 10001
// ���� ����:
HINSTANCE hInst;                                // ���� �ν��Ͻ��Դϴ�.
WCHAR szTitle[MAX_LOADSTRING];                  // ���� ǥ���� �ؽ�Ʈ�Դϴ�.
WCHAR szWindowClass[MAX_LOADSTRING];            // �⺻ â Ŭ���� �̸��Դϴ�.

// �� �ڵ� ��⿡ ��� �ִ� �Լ��� ������ �����Դϴ�.
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: ���⿡ �ڵ带 �Է��մϴ�.

    // ���� ���ڿ��� �ʱ�ȭ�մϴ�.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_GUISENDFILESERVER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // ���� ���α׷� �ʱ�ȭ�� �����մϴ�.
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GUISENDFILESERVER));

    MSG msg;

    // �⺻ �޽��� �����Դϴ�.
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  �Լ�: MyRegisterClass()
//
//  ����: â Ŭ������ ����մϴ�.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GUISENDFILESERVER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_GUISENDFILESERVER);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   �Լ�: InitInstance(HINSTANCE, int)
//
//   ����: �ν��Ͻ� �ڵ��� �����ϰ� �� â�� ����ϴ�.
//
//   ����:
//
//        �� �Լ��� ���� �ν��Ͻ� �ڵ��� ���� ������ �����ϰ�
//        �� ���α׷� â�� ���� ���� ǥ���մϴ�.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // �ν��Ͻ� �ڵ��� ���� ������ �����մϴ�.

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 600, 300, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  �Լ�: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ����:  �� â�� �޽����� ó���մϴ�.
//
//  WM_COMMAND  - ���� ���α׷� �޴��� ó���մϴ�.
//  WM_PAINT    - �� â�� �׸��ϴ�.
//  WM_DESTROY  - ���� �޽����� �Խ��ϰ� ��ȯ�մϴ�.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	char tempBuff[100];
	static char pcForSaveFilename[1000];
	//---------------------------------���α׷����� ���� ����
	INITCOMMONCONTROLSEX initCtrlSex;
	initCtrlSex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	initCtrlSex.dwICC = ICC_PROGRESS_CLASS|ICC_INTERNET_CLASSES;
	static HWND h_ProgressbarCtrl;
	InitCommonControlsEx(&initCtrlSex);
	//---------------------------------���α׷����� ���� ����
	
	//  ----------------------------------------------���Ͽ����� ���� �����ȭ���ں���.
	char str[100], lpstrFile[100] = "";
	char filter[] = "Every File(*.*)\0*.*\0 Text File\0*.txt;*.doc\0";
	static OPENFILENAMEA OFN;
	memset(&OFN, 0, sizeof(OPENFILENAME));
	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner = hWnd;
	OFN.lpstrFilter = filter;
	OFN.lpstrFile = lpstrFile;
	OFN.nMaxFile = 100;
	OFN.lpstrInitialDir = "."; // �ʱ� ���丮
	//  ----------------------------------------------���Ͽ����� ���� �����ȭ���ں���.

	//---------------------------------------------IP�ּ� ��Ʈ���� ���� ����
	HWND h_ipAddressCtrl;

	DWORD ipdw;
	SOCKADDR_IN serverAddr;
	//---------------------------------------------IP�ּ� ��Ʈ���� ���� ����
	static std::thread* mythread;
    switch (message)
    {
	case WM_CREATE:
		h_ProgressbarCtrl = CreateWindowEx(0, PROGRESS_CLASS, NULL, WS_VISIBLE | WS_CHILD, 20, 20,500, 50, hWnd, NULL, hInst, NULL);
		//SendMessage(h_ProgressbarCtrl, PBM_SETPOS, (WPARAM)40,0); --- �ۼ������� ä���.;
		h_ipAddressCtrl = CreateWindow(WC_IPADDRESS,"", WS_CHILD | WS_OVERLAPPED | WS_VISIBLE, 20,70,300, 30, hWnd, (HMENU)IDC_IPADDR, hInst, 0);
		CreateWindow(WC_BUTTON, "Send", WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE, 20, 100, 300, 30, hWnd,(HMENU)ID_BUTTON, hInst, 0);
		// �޼��� ������ .
		break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // �޴� ������ ���� �м��մϴ�.
            switch (wmId)
            {
			case ID_BUTTON:
				SendMessage(GetDlgItem(hWnd,IDC_IPADDR), IPM_GETADDRESS, 0, (LPARAM)&ipdw);
				serverAddr.sin_addr.S_un.S_addr = htonl(ipdw);
				wsprintf(tempBuff, "%s\n,%s", inet_ntoa(serverAddr.sin_addr),pcForSaveFilename);

				if (MessageBox(hWnd, tempBuff, "�������� ����?", MB_YESNO) == IDYES)
				{
					//������ ����
					mythread = new std::thread(ThreadFunc, h_ProgressbarCtrl, serverAddr, pcForSaveFilename);
					//�Ķ���� : ���α׷������� �ڵ�. �Է¹��� IP�ּ�,�����̸�,

				}
				break;
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
			case ID_FILEOPEN:
				if (GetOpenFileNameA(&OFN) != 0) {
					//wsprintfA(str, "%s ������ ���ðڽ��ϱ� ?", OFN.lpstrFile);
					//MessageBox(hWnd, str, "���� ����", MB_OK);
					strcpy(pcForSaveFilename, OFN.lpstrFile);
				}
				break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: ���⿡ hdc�� ����ϴ� �׸��� �ڵ带 �߰��մϴ�.
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
		mythread->join();
		delete mythread;
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// ���� ��ȭ ������ �޽��� ó�����Դϴ�.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	
	UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;
	case WM_CREATE:
		
		break;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
