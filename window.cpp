#include <windows.h>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>

using namespace std;

#define FILE_MENU_EXIT 1
#define INITIALIZE_PRESSED 2

#define STATUS_XINITIAL 10
#define STATUS_YINITIAL 10
#define STATUS_YINCREMENT 40
#define STATUS_HEIGHT 38
#define STATUS_WIDTH 148

#define RESULT_XINITIAL 160
#define RESULT_YINITIAL 10
#define RESULT_YINCREMENT 40
#define RESULT_HEIGHT 38
#define RESULT_WIDTH 173

#define DEFAULT_TEXT "SNsanafonP"

#define P1_PATH "C:/Program Files/Cockatrice/servatrice.exe"
#define P2_PATH "NUL"
#define P3_PATH "NUL"

#define P1_TITLE "Kontakt 5"
#define P2_TITLE 2	//NULL
#define P3_TITLE 3	//NULL

//forward declarations
LRESULT CALLBACK WProc(HWND, UINT, WPARAM, LPARAM);

static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM param);
static BOOL CALLBACK EnumWindowsChildProc(HWND hwnd, LPARAM title);
HWND GetHwnd(DWORD threadid, LPARAM title);

void AddMenus(HWND);
void InitExProgram(HWND);
void SetFont(HWND hWnd, HWND hTarget,int size);
void LaunchExProgram();
void WinLayout();

//global variables
HMENU hMenu;
HWND hP1, hP2, hP3, hP1Result, hP2Result, hP3Result, InitButton, StatusBar, P1, P2, P3, g_HWND;
HFONT hFont;
HDC hdc;
FILE* WinFile;


STARTUPINFO startInfoP1, startInfoP2, startInfoP3 = { 0 };
PROCESS_INFORMATION processInfoP1, processInfoP2, processInfoP3 = { 0 };

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow) {
	

	//initialize window class
	WNDCLASSW wc = { 0 };
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hInstance = hInst;
	wc.lpszClassName = (LPCWSTR)(LPCWSTR)"InitWindow";
	wc.lpfnWndProc = WProc;

	//check for successful class registration
	if (!RegisterClassW(&wc)) {
		return -1;
	}

	CreateWindowW((LPCWSTR)(LPCWSTR)"InitWindow", (LPCWSTR)"AutoCAP", WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME | WS_VISIBLE,
				  100, 25, 500, 500, NULL, NULL, NULL, NULL);
	MSG msg = { 0 };

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);

	}

	return 0;
}

LRESULT CALLBACK WProc(HWND hWnd, UINT msg, WPARAM  wp, LPARAM lp) {
	switch (msg) {
	case WM_DESTROY:
		if (!FreeConsole()) {
			SetWindowText(StatusBar, "Console Destruction Failed!");
		}
		fclose(WinFile);
		PostQuitMessage(0);
		break;
	case WM_CREATE:
		AddMenus(hWnd);
		InitExProgram(hWnd);
		break;
	case WM_COMMAND:
		switch (wp) {
		case FILE_MENU_EXIT:
			DestroyWindow(hWnd);
			break;
		case INITIALIZE_PRESSED:
			LaunchExProgram();
			WinLayout();
			break;
		}
		break;
	default:
		return DefWindowProcW(hWnd,msg,wp,lp);
	}
}

void AddMenus(HWND hWnd) {
	hMenu = CreateMenu();
	HMENU hFileMenu = CreateMenu();

	AppendMenu(hFileMenu, MF_STRING, FILE_MENU_EXIT, "Exit");

	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, (LPCSTR)"File");

	SetMenu(hWnd,hMenu);
}

void InitExProgram(HWND hWnd) {
	//create labels (static)
	hP1 = CreateWindowExW(WS_EX_STATICEDGE, (LPCWSTR)"static", (LPCWSTR)"P1 Status:", WS_VISIBLE | WS_CHILD | WS_BORDER, STATUS_XINITIAL, STATUS_YINITIAL, STATUS_WIDTH, STATUS_HEIGHT, hWnd, NULL, NULL, NULL);
	SetFont(hWnd, hP1, 24);
	hP2 = CreateWindowExW(WS_EX_STATICEDGE, (LPCWSTR)"static", (LPCWSTR)"P2 Status:", WS_VISIBLE | WS_CHILD | WS_BORDER, STATUS_XINITIAL, STATUS_YINITIAL + STATUS_YINCREMENT, STATUS_WIDTH, STATUS_HEIGHT, hWnd, NULL, NULL, NULL);
	SetFont(hWnd, hP2, 24);
	hP3 = CreateWindowExW(WS_EX_STATICEDGE, (LPCWSTR)"static", (LPCWSTR)"P3 Status:", WS_VISIBLE | WS_CHILD | WS_BORDER, STATUS_XINITIAL, STATUS_YINITIAL + 2 * STATUS_YINCREMENT, STATUS_WIDTH, STATUS_HEIGHT, hWnd, NULL, NULL, NULL);
	SetFont(hWnd, hP3, 24);
	//create status (dynamic)
	hP1Result = CreateWindowExW(WS_EX_STATICEDGE, (LPCWSTR)"static", (LPCWSTR)"Deinitialized", SS_CENTER | WS_VISIBLE | WS_CHILD | WS_BORDER, RESULT_XINITIAL, RESULT_YINITIAL, RESULT_WIDTH, RESULT_HEIGHT, hWnd, NULL, NULL, NULL);
	SetFont(hWnd, hP1Result, 24);
	hP2Result = CreateWindowExW(WS_EX_STATICEDGE, (LPCWSTR)"static", (LPCWSTR)"Deinitialized", SS_CENTER | WS_VISIBLE | WS_CHILD | WS_BORDER, RESULT_XINITIAL, RESULT_YINITIAL + RESULT_YINCREMENT, RESULT_WIDTH, RESULT_HEIGHT, hWnd, NULL, NULL, NULL);
	SetFont(hWnd, hP2Result, 24);
	hP3Result = CreateWindowExW(WS_EX_STATICEDGE, (LPCWSTR)"static", (LPCWSTR)"Deinitialized", SS_CENTER | WS_VISIBLE | WS_CHILD | WS_BORDER, RESULT_XINITIAL, RESULT_YINITIAL + 2 * RESULT_YINCREMENT, RESULT_WIDTH, RESULT_HEIGHT, hWnd, NULL, NULL, NULL);
	SetFont(hWnd, hP3Result, 24);
	//create init button
	InitButton = CreateWindowExW(0, (LPCWSTR)"button", (LPCWSTR)"Initialize", BS_DEFPUSHBUTTON | SS_CENTER | WS_VISIBLE | WS_CHILD | WS_BORDER, 335, 10, 148, 50, hWnd, (HMENU)INITIALIZE_PRESSED, NULL, NULL);
	SetFont(hWnd, InitButton, 24);
	//create status bar
	StatusBar = CreateWindowExW(WS_EX_STATICEDGE, (LPCWSTR)"static", (LPCWSTR)"Waiting for Input", WS_VISIBLE | WS_CHILD, 5, 420, 500, 23, hWnd, NULL, NULL, NULL);
	SetFont(hWnd, StatusBar, 12);
}

void SetFont(HWND hWnd,HWND hTarget,int size){
	long lfHeight;
	
	hdc = GetDC(NULL);
	lfHeight = -MulDiv(size, GetDeviceCaps(hdc, LOGPIXELSY), 72);
	ReleaseDC(NULL, hdc);

	hFont = CreateFont(lfHeight, 0, 0, 0, 0, FALSE, 0, 0, 0, 0, 0, 0, 0, DEFAULT_TEXT);

	if (!hFont) {
		MessageBox(hWnd, (LPCSTR)"Font creation failed!", (LPCSTR)"Error", MB_OK | MB_ICONEXCLAMATION);
	}
	SendMessageW(hTarget, WM_SETFONT, WPARAM(hFont), TRUE);
}

void LaunchExProgram() {
	SetWindowText(StatusBar, (LPCSTR)"Beginning Initialization");

	//init P1
	if (CreateProcess(TEXT(P1_PATH), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &startInfoP1, &processInfoP1)) {
		SetWindowText(hP1Result, (LPCSTR)"Initialized");
	}
	else {
		if (P1_PATH != "NUL") {
			SetWindowText(StatusBar, "P1 Failed!");
		}
	}
	//init P2
	if (CreateProcess(TEXT(P2_PATH), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &startInfoP2, &processInfoP2)) {
		SetWindowText(hP2Result, (LPCSTR)"Initialized");
	}
	else {
		if (P2_PATH != "NUL") {
			SetWindowText(StatusBar, "P2 Failed!");
		}
	}
	//init P3
	if (CreateProcess(TEXT(P3_PATH), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &startInfoP3, &processInfoP3)) {
		SetWindowText(hP3Result, (LPCSTR)"Initialized");
	}
	else {
		if (P3_PATH != "NUL") {
			SetWindowText(StatusBar, "P3 Failed!");
		}
	}
}

void WinLayout() {


	if (!AllocConsole()) {
		SetWindowText(StatusBar, "Console Initialization Failed!");
	}

	//freopen_s(&WinFile, "CONOUT$", "w", stdout);						//workaround to manipulate window position
	cout.clear();
	Sleep(500);


	P1 = GetHwnd(processInfoP1.dwThreadId, 0);
	cout << "test";
	cout << P1 << endl;

	Sleep(2000);

	if (!SetWindowPos(P1, NULL, 250, 500, 5, 5, SWP_NOSIZE | SWP_SHOWWINDOW)) {
		//SetWindowText(StatusBar, "P3 Move Failed!");
	}

}

HWND GetHwnd(DWORD threadid, LPARAM title) {
	HWND handle;
	if (EnumWindows(EnumWindowsProc, threadid)) {
		SetWindowText(StatusBar, "Window Enumeration Failed");
	}

	/*if(EnumChildWindows(g_HWND, EnumWindowsChildProc, title)){
		SetWindowText(StatusBar, "Child Window Enumeration Failed");
	}*/
	cout << g_HWND << endl;

	//g_HWND = FindWindowExA(g_HWND, NULL, "NINormalWindow0000000140000000", P1_TITLE);

	if (!FindWindowA("NINormalWindow0000000140000000", P1_TITLE)) {
		g_HWND = FindWindowA("NINormalWindow0000000140000000", P1_TITLE);
	}
	else {
		SetWindowText(StatusBar, "Child Window Finder Failed!");
	}

	return g_HWND;
}


static BOOL CALLBACK EnumWindowsChildProc(HWND hwnd, LPARAM title) {
	
	//get title from hwnd, compare it to lparam title
	/*s
	LPSTR hTitle;

	if (GetWindowTextA(hwnd, hTitle, 100) == 0) {
		SetWindowText(StatusBar, "GetWindowTextA Failed!");
	}

	cout << g_HWND;

	if (hTitle == (LPSTR)title) {
		g_HWND = hwnd;
		return false;
	}
	else {
		return true;
	}*/
}


static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM param){

	DWORD id = GetWindowThreadProcessId(hwnd, &id);
	if (id == (DWORD)param){
		g_HWND = hwnd;
		return false;
	}
	else {
		return true;
	}
}