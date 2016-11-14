// GUISendFile(server).cpp : 응용 프로그램에 대한 진입점을 정의합니다.
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
// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
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

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_GUISENDFILESERVER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 응용 프로그램 초기화를 수행합니다.
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GUISENDFILESERVER));

    MSG msg;

    // 기본 메시지 루프입니다.
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
//  함수: MyRegisterClass()
//
//  목적: 창 클래스를 등록합니다.
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
//   함수: InitInstance(HINSTANCE, int)
//
//   목적: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   설명:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

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
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  목적:  주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 응용 프로그램 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	char tempBuff[100];
	static char pcForSaveFilename[1000];
	//---------------------------------프로그래스바 전용 변수
	INITCOMMONCONTROLSEX initCtrlSex;
	initCtrlSex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	initCtrlSex.dwICC = ICC_PROGRESS_CLASS|ICC_INTERNET_CLASSES;
	static HWND h_ProgressbarCtrl;
	InitCommonControlsEx(&initCtrlSex);
	//---------------------------------프로그래스바 전용 변수
	
	//  ----------------------------------------------파일오픈을 위한 공용대화상자변수.
	char str[100], lpstrFile[100] = "";
	char filter[] = "Every File(*.*)\0*.*\0 Text File\0*.txt;*.doc\0";
	static OPENFILENAMEA OFN;
	memset(&OFN, 0, sizeof(OPENFILENAME));
	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner = hWnd;
	OFN.lpstrFilter = filter;
	OFN.lpstrFile = lpstrFile;
	OFN.nMaxFile = 100;
	OFN.lpstrInitialDir = "."; // 초기 디렉토리
	//  ----------------------------------------------파일오픈을 위한 공용대화상자변수.

	//---------------------------------------------IP주소 컨트롤을 위한 변수
	HWND h_ipAddressCtrl;

	DWORD ipdw;
	SOCKADDR_IN serverAddr;
	//---------------------------------------------IP주소 컨트롤을 위한 변수
	static std::thread* mythread;
    switch (message)
    {
	case WM_CREATE:
		h_ProgressbarCtrl = CreateWindowEx(0, PROGRESS_CLASS, NULL, WS_VISIBLE | WS_CHILD, 20, 20,500, 50, hWnd, NULL, hInst, NULL);
		//SendMessage(h_ProgressbarCtrl, PBM_SETPOS, (WPARAM)40,0); --- 퍼센테이지 채우기.;
		h_ipAddressCtrl = CreateWindow(WC_IPADDRESS,"", WS_CHILD | WS_OVERLAPPED | WS_VISIBLE, 20,70,300, 30, hWnd, (HMENU)IDC_IPADDR, hInst, 0);
		CreateWindow(WC_BUTTON, "Send", WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE, 20, 100, 300, 30, hWnd,(HMENU)ID_BUTTON, hInst, 0);
		// 메세지 보내기 .
		break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다.
            switch (wmId)
            {
			case ID_BUTTON:
				SendMessage(GetDlgItem(hWnd,IDC_IPADDR), IPM_GETADDRESS, 0, (LPARAM)&ipdw);
				serverAddr.sin_addr.S_un.S_addr = htonl(ipdw);
				wsprintf(tempBuff, "%s\n,%s", inet_ntoa(serverAddr.sin_addr),pcForSaveFilename);

				if (MessageBox(hWnd, tempBuff, "이정보가 맞음?", MB_YESNO) == IDYES)
				{
					//스레드 생성
					mythread = new std::thread(ThreadFunc, h_ProgressbarCtrl, serverAddr, pcForSaveFilename);
					//파라미터 : 프로그래스바의 핸들. 입력받은 IP주소,파일이름,

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
					//wsprintfA(str, "%s 파일을 여시겠습니까 ?", OFN.lpstrFile);
					//MessageBox(hWnd, str, "열기 선택", MB_OK);
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
            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다.
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

// 정보 대화 상자의 메시지 처리기입니다.
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
