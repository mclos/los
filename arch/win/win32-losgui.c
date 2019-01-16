/*----------------------------------------------------------------------------/
 /  los -  system module  R0.1
 /-----------------------------------------------------------------------------/
 /
 / Copyright (c) 2014-2017 LP电子,All Rights Reserved.
 /
 / 未经授权，禁止商用。
 /----------------------------------------------------------------------------*/

#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include <stdint.h>
#include <process.h>
#include "los.h"
#include "lcdDriver.h"
lgui_t *lgui;
// Global variables
uint8_t flg = 0;
uint16_t win_width = 250;
uint16_t win_height = 420;
COLORREF *lcd_buf;
// The main window class name.
static TCHAR szWindowClass[] = _T("win32app");
 
static TCHAR szTitle[] = _T("Win los");
extern void win_time_init();
extern uint32_t los_app_frist(uint8_t *pname);
extern int comm_arg_deal(char *com, uint8_t ***buf);
char pname[128] = {0};
char papp[128] = {0};
void fun1(void *v)
{
	int res = 0;
	char data[128];
	 
	win_time_init(); //是los的time管理，初始化
	los_app_mini(1);
	//los_set_api(LOS_LGUI_API_TYPE, los_lgui_api);
	//los_set_api(LOS_USER_API_TYPE, los_user_api);
	if (pname[0] == 0)
		res = los_app_main(0);
	else if (strcmp(pname, "app") == 0)
	{
		if (papp[0])  
			los_appname_frist(papp);
		else
			res = los_app_main(1);
	}
	else
	{
 		res = los_app_frist(pname);
	}
	printf("los res=%x\n", res);
	sprintf(data, "run ret %x\n", res);
	MessageBox(NULL, data, L"Error", MB_OK);
	exit(1);
}
int CommandLineToArgvA(char *v, char *pNumArgs)
{
	uint8_t buf[128];
	int len = 0, ff = 0, flg = 0, i = 0;
	len = strlen(v);
 	memset(buf, 0, 128);
	strcpy(buf, v);  
	if (v[0] == '"')
	{
		i = 1;
		ff = 1;
	}
	for (; i < len; ++i)
	{
		if (ff) //是 “”
		{
			if (v[i] == '"')
			{
				memcpy(buf, &v[1], i - 1);
				break;
			}
		}
		else //这是空格
		{
			if (v[i] == ' ')
			{
				memcpy(buf, v, i);
				buf[i] = 0;  
				break;
			}
		}
	}
	get_pc_los_path(buf);
	uint8_t **bufs, num = 0;
	num = comm_arg_deal(pNumArgs, &bufs);
	for (i = 0; i < num; ++i)
	{
		if (i == 0)
			strcpy(pname, bufs[0]);
		else if (i == 1)
			strcpy(papp, bufs[1]);
		free(bufs[i]);
	}

	return 0;
}

/**
 * los
 * app tick
 * */
extern uint8_t los_app_tick(void);
void func_tick(void *v)
{
	Sleep(10);
	while (1)
	{
		los_app_tick();
		Sleep(1);
	}
	exit(1);
}
extern void win_time_tick();
void func_time(void *v)
{
	Sleep(10);
	while (1)
	{
		win_time_tick();
		Sleep(1000);
	}
	exit(1);
}
extern int ft_test(void);
// Forward declarations of functions included in this code module:
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HWND mhWnd;
void window_set_los_title(char *name)
{
	SetWindowText(mhWnd, name);
}
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
					 LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wcex;

	HINSTANCE hInst;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

	if (!RegisterClassEx(&wcex))
	{
		MessageBox(NULL, _T("Call to RegisterClassEx failed!"),
				   _T("Win32 Guided Tour"),
				   NULL);

		return 1;
	}
	LPWSTR *szArgList;
	int argCount;

	szArgList = CommandLineToArgvW(GetCommandLine(), &argCount);
	if (szArgList == NULL)
	{
		MessageBox(NULL, L"Unable to parse command line", L"Error", MB_OK);
		return 10;
	}

	CommandLineToArgvA(szArgList[0], lpCmdLine); //get cmd
	hInst = hInstance;							 // Store instance handle in our global variable
 
	mhWnd = CreateWindow(
		szWindowClass,
		szTitle,
		WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT, //600,100,
		win_width, win_height,
		NULL,
		NULL,
		hInstance,
		NULL);
 	if (!mhWnd)
	{
		MessageBox(NULL, _T("Call to CreateWindow failed!"),
				   _T("Win32 Guided Tour"),
				   NULL);

		return 1;
	}
	LocalFree(szArgList);
    lgui=los_get_io();
 
	int scrWidth, scrHeight;
	RECT rect;
	lcd_buf = malloc(win_width * win_height * sizeof(COLORREF));
	if (lcd_buf == 0)
		return 1;
	memset(lcd_buf, 240, win_width * win_height * sizeof(COLORREF)); //clear

	scrWidth = GetSystemMetrics(SM_CXSCREEN);
	scrHeight = GetSystemMetrics(SM_CYSCREEN);
	GetWindowRect(mhWnd, &rect);
	SetWindowPos(mhWnd, HWND_TOPMOST, (scrWidth - rect.right) / 2, (scrHeight - rect.bottom) / 2, rect.right - rect.left, rect.bottom - rect.top, SWP_SHOWWINDOW);
 
	_beginthread(fun1, 0, NULL);
	SetTimer(mhWnd, 1, 10, NULL);
 	_beginthread(func_time, 0, NULL);
	_beginthread(func_tick, 0, NULL);
	ShowWindow(mhWnd, nCmdShow);
	UpdateWindow(mhWnd);
 	MSG msg; // Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}
uint16_t recx, recy, recw, rech;
void Draw(HDC hdc)
{
	int x, y;
	for (y = 0; y < win_height; ++y)
		for (x = 0; x < win_width; ++x)
		{
			SetPixel(hdc, x, y, lcd_buf[x + win_width * y]);
		}
	recx = win_width / 2 - 20;
	recy = win_height - 75;
	recw = recx + 40;
	rech = recy + 40;

	RoundRect(hdc, recx, recy, recw, rech, 5, 5);
	RoundRect(hdc, recx + 3, recy + 3, recw - 3, rech - 3, 5, 5);
 
}

int CaptureAnImage(HWND hwnd)
{
	PAINTSTRUCT ps;
	HDC hdc_old = BeginPaint(hwnd, &ps);
	HDC hdc = CreateCompatibleDC(hdc_old);
	RECT clientRect;
	GetClientRect(hwnd, &clientRect);
	HBITMAP hBmp = CreateCompatibleBitmap(hdc_old, clientRect.right,
										  clientRect.bottom);

	SelectObject(hdc, hBmp);
	SelectObject(hdc, GetSysColorBrush(COLOR_3DFACE));
	Rectangle(hdc, -1, -1, clientRect.right + 2, clientRect.bottom + 2);

	Draw(hdc);

	BitBlt(hdc_old, 0, 0, clientRect.right, clientRect.bottom, hdc, 0, 0,
		   SRCCOPY);
	DeleteObject(hBmp);
	DeleteDC(hdc);
	ReleaseDC(hwnd, hdc);
	EndPaint(hwnd, &ps);
	return 0;
}

uint32_t re_time = 0;
int8_t point_up = 0, sync = 0;
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int nums = 0;
	switch (message)
	{
	case WM_CLOSE:
		printf((const char *)"app exit!\n");
		exit(1);
		break;
	case WM_TIMER:

		switch (wParam)
		{

		case 1: //ui refresh 50ms
			if (flg == 2)
			{
				re_time = 0;
				RedrawWindow(hWnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE);
				flg = 0;
			}
			else if (flg)
			{
				re_time++;
				if (re_time > 3) //10*10 
				{
					re_time = 0;
					RedrawWindow(hWnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE);
					flg = 0;
				}
			}
			break;
		case 2:
			los_app_tick();
			break;
		}
		break;
	case WM_CREATE:
	{
		break;
	}
	case WM_LBUTTONDOWN:
		lgui->point_x = LOWORD(lParam); //x
		lgui->point_y = HIWORD(lParam); //y
 		lgui->point_flgs = 0;
		nums = 0;
		while (lgui->point_flgs != -1)
		{
			Sleep(5);
			nums++;
			if (nums > 20)
				break;
		}
		if (lgui->point_x > recx && lgui->point_x < recw && lgui->point_y > recy && lgui->point_y < rech)
		{
			los_app_min();
		}
		break;
	case WM_LBUTTONUP:
		Sleep(5);
		lgui->point_flgs = 1;
		break;
	case WM_PAINT:
		CaptureAnImage(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
 
		switch (wParam)
		{
		case VK_UP:
			lgui->key_key = VK_UP;
 			break;
		case VK_DOWN:
			lgui->key_key = VK_DOWN;
 			break;
		case VK_RETURN:
			lgui->key_key = VK_RETURN;
 			break;
		}
		break;
	case WM_ERASEBKGND:
		return TRUE;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}
	return 0;
}
