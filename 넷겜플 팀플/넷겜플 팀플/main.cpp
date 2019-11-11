#include <Windows.h>
#include <vector>
#include <atlImage.h>
#include <math.h>
#include <mmsystem.h>
#include "Utill.h"
#include "Enemy.h"
#pragma comment(lib,"winmm.lib") 

using namespace std;

#define WndX 1080	
#define WndY 720

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
LPCTSTR lpszClass = "ApiBase";

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance
	, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst = hInstance;

	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = (WNDPROC)WndProc;
	WndClass.lpszClassName = lpszClass;
	WndClass.lpszMenuName = NULL;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);

	srand((unsigned int)time(NULL)); // 랜덤 시드값

	hWnd = CreateWindow(
		lpszClass, // 윈도우 클래스 이름
		"넷겜플 팀플", // 윈도우 타이틀 이름
		WS_OVERLAPPEDWINDOW, // 윈도우 스타일
		220, // 윈도우 위치, x좌표 
		70, // 윈도우 위치, y좌표
		WndX, // 윈도우 가로(폭) 크기 
		WndY, // 윈도우 세로(높이) 크기
		NULL, // 부모 윈도우 핶들
		NULL, // 메뉴 핶들
		hInstance, // 응용 프로그램 인스턴스
		NULL);   // 생성된 윈도우 정보
	ShowWindow(hWnd, nCmdShow);

	while (GetMessage(&Message, 0, 0, 0))
	{
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMessage, WPARAM
	wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc, mem0dc;
	static HBITMAP hbmOld, hbmMem, hbmMemOld;			// 더블버퍼링을 위하여!
	static RECT rt;

	switch (iMessage) {
	case WM_CREATE:
		srand((unsigned int)time(NULL));

		GetClientRect(hwnd, &rt);
		init_Monster_Image();
		
		break;

	case WM_CHAR:
		break;

	case WM_LBUTTONDOWN:
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_UP:
			break;
		case VK_DOWN:
			break;
		case VK_LEFT:
			break;
		case VK_RIGHT:
			break;
		case VK_SPACE:
			break;
		case VK_ESCAPE:
			break;
		case 'Q':
			PostQuitMessage(0);
			break;
		}
		InvalidateRect(hwnd, NULL, false);
		break;

	case WM_KEYUP:
		switch (wParam)
		{
		case VK_UP:
			break;
		case VK_DOWN:
			break;
		case VK_LEFT:
			break;
		case VK_RIGHT:
			break;
		case VK_SPACE:
			break;
		}
		InvalidateRect(hwnd, NULL, false);
		break;

	case WM_TIMER:
		switch (wParam)
		{
		case 1:
			break;

		case 2:
			break;

		case 3:
			break;

		case 4:
			break;

		case 5:
			break;

		case 6:
			break;

		case 7:
			break;

		case 8:
			break;

		case 9:
			break;

		case 10:

			break;
		}
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		mem0dc = CreateCompatibleDC(hdc);//2
		hbmMem = CreateCompatibleBitmap(hdc, rt.right, rt.bottom);//3
		SelectObject(mem0dc, hbmMem);
		PatBlt(mem0dc, rt.left, rt.top, rt.right, rt.bottom, BLACKNESS);
		hbmMemOld = (HBITMAP)SelectObject(mem0dc, hbmMem);//4

		//Monster_Draw(mem0dc, 199, 579, 0, 100);
		draw_enemy(mem0dc);
		

		BitBlt(hdc, 0, 0, rt.right, rt.bottom, mem0dc, 0, 0, SRCCOPY);

		SelectObject(mem0dc, hbmMemOld); //-4
		DeleteObject(hbmMem); //-3
		DeleteObject(hbmMemOld); //-3
		DeleteDC(mem0dc); //-2
		DeleteDC(hdc); //-2
		EndPaint(hwnd, &ps);
		break;

	case WM_DESTROY:

		PostQuitMessage(0);
		break;
	}
	return(DefWindowProc(hwnd, iMessage, wParam, lParam));
}

