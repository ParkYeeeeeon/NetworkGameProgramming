#include <Windows.h>
#include <vector>
#include <atlImage.h>
#include <math.h>
#include <mmsystem.h>
#include "Utill.h"
#include "Enemy.h"
#include "Player.h"
#include "GameState.h"
#pragma comment(lib,"winmm.lib") 

using namespace std;

#define WndX 1080	
#define WndY 720

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
LPCTSTR lpszClass = "ApiBase";

Player PLAYER[2];
UI ui;
CImage mapimg;

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
	HWND cpy_hwnd;
	static HBITMAP hbmOld, hbmMem, hbmMemOld;			// 더블버퍼링을 위하여!
	static RECT rt;

	switch (iMessage) {
	case WM_CREATE:
		srand((unsigned int)time(NULL));

		cpy_hwnd = hwnd;

		GetClientRect(hwnd, &rt);

		mapimg.Load("Image\\Map\\Background.png");
		PLAYER[0].player_img.Load("Image\\Player\\Player.png");
		PLAYER[0].player_up_img.Load("Image\\Player\\Player_up.png");
		PLAYER[0].player_down_img.Load("Image\\Player\\Player_down.png");
		PLAYER[0].control = PLAYER_ME;
		
		init_Monster_Image();
		init_ui(ui);
		init_Monster_Bullet_Image();	// 이미지를 초기화 시킨다.

		// 몬스터 애니메이션 변경 타이머
		//SetTimer(cpy_hwnd, 1, 1024, NULL);	// 1번 타이머를 1초간(1024ms) 움직인다
		//SetTimer(cpy_hwnd, 2, 100, NULL);	// 2번 타이머를 0.1초간(100ms) 움직인다
		//SetTimer(cpy_hwnd, 3, 10, NULL);	// 3번 타이머를 0.1초간(100ms) 움직인다
		//SetTimer(cpy_hwnd, 4, 1024, NULL);	// 4번 타이머를 1초간(1024ms) 움직인다
		
		SetTimer(cpy_hwnd, 5, 10, NULL);	// 플레이어 이동 타이머
		break;
		
	

	case WM_CHAR:
		break;

	case WM_LBUTTONDOWN:
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_UP:
			for (int i = 0; i < 2; ++i) {
				if (PLAYER[i].control == PLAYER_ME) {
					PLAYER[i].moveY = 2;
				}
			}
			
			break;
		case VK_DOWN:
			for (int i = 0; i < 2; ++i) {
				if (PLAYER[i].control == PLAYER_ME) {
					PLAYER[i].moveY = 1;
				}
			}
			
			break;
		case VK_LEFT:
			for (int i = 0; i < 2; ++i) {
				if (PLAYER[i].control == PLAYER_ME) {
					PLAYER[i].moveX = 2;
				}
			}
			break;
		case VK_RIGHT:
			for (int i = 0; i < 2; ++i) {
				if (PLAYER[i].control == PLAYER_ME) {
					PLAYER[i].moveX = 1;
				}
			}
			break;
		case VK_SPACE:
			key_input(VK_SPACE);
			break;
		case VK_ESCAPE:
			key_input(VK_ESCAPE);
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
			for (int i = 0; i < 2; ++i) {
				if (PLAYER[i].control == PLAYER_ME) {
					PLAYER[i].moveY = 0;
				}
			}
			break;
		case VK_DOWN:
			for (int i = 0; i < 2; ++i) {
				if (PLAYER[i].control == PLAYER_ME) {
					PLAYER[i].moveY = 0;
				}
			}
			break;
		case VK_LEFT:
			for (int i = 0; i < 2; ++i) {
				if (PLAYER[i].control == PLAYER_ME) {
					PLAYER[i].moveX = 0;
				}
			}
			break;
		case VK_RIGHT:
			for (int i = 0; i < 2; ++i) {
				if (PLAYER[i].control == PLAYER_ME) {
					PLAYER[i].moveX = 0;
				}
			}
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
			// 몬스터 애니메이션 변화
			for (int i = 0; i < MONSTER_COUNT; ++i) {
				change_enemy_ani(0, i);
				change_enemy_ani(1, i);
				change_enemy_ani(2, i);
			}
			break;

		case 2:
			// 몬스터 위치 변화
			for (int i = 0; i < MONSTER_COUNT; ++i)
			{
				change_enemy_location(0, i);
				change_enemy_location(1, i);
				change_enemy_location(2, i);
			}
			break;

		case 3:
			// 몬스터 위치 변화
			for (int i = 0; i < MONSTER_COUNT; ++i)
			{
				change_enemy_bullet(0, i);
				change_enemy_bullet(1, i);
				change_enemy_bullet(2, i);
			}
			break;

		case 4:
			// 몬스터 총알 추가 생성
			for (int i = 0; i < MONSTER_COUNT; i++) {
				for (int j = 0; j < 1; ++j) {
					Bullet bullet;
					bullet.position.x = mon1[i].position.x;
					bullet.position.y = mon1[i].position.y;
					bullet.type = 0;
					mon1[i].bullet.push_back(bullet);
				}
			}
			break;

		case 5:
			for (int i = 0; i < 2; ++i) {
				if (PLAYER[i].control == PLAYER_ME) {
					switch (PLAYER[i].moveX) {
					case 1: 
						PLAYER[i].position.x += 5;
						break;
					case 2:
						PLAYER[i].position.x -= 5;
						break;
					}
					switch (PLAYER[i].moveY) {
					case 1:
						PLAYER[i].position.y += 5;
						break;
					case 2:
						PLAYER[i].position.y -= 5;
						break;
					}
				}
			}
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
		InvalidateRgn(hwnd, NULL, FALSE);
		break;

	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		mem0dc = CreateCompatibleDC(hdc);//2
		hbmMem = CreateCompatibleBitmap(hdc, rt.right, rt.bottom);//3
		SelectObject(mem0dc, hbmMem);
		PatBlt(mem0dc, rt.left, rt.top, rt.right, rt.bottom, BLACKNESS);
		hbmMemOld = (HBITMAP)SelectObject(mem0dc, hbmMem);//4

		//Monster_Draw(mem0dc, 199, 579, 0, 100);
		draw_map(mem0dc, mapimg);
		
		draw_enemy(mem0dc);
		draw_player(mem0dc,PLAYER);
		draw_enemybullet(mem0dc);	// 총알을 그린다.
		
		draw_ui(mem0dc, ui);

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

