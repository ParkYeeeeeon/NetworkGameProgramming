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

	srand((unsigned int)time(NULL)); // ���� �õ尪

	hWnd = CreateWindow(
		lpszClass, // ������ Ŭ���� �̸�
		"�ݰ��� ����", // ������ Ÿ��Ʋ �̸�
		WS_OVERLAPPEDWINDOW, // ������ ��Ÿ��
		220, // ������ ��ġ, x��ǥ 
		70, // ������ ��ġ, y��ǥ
		WndX, // ������ ����(��) ũ�� 
		WndY, // ������ ����(����) ũ��
		NULL, // �θ� ������ �L��
		NULL, // �޴� �L��
		hInstance, // ���� ���α׷� �ν��Ͻ�
		NULL);   // ������ ������ ����
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
	static HBITMAP hbmOld, hbmMem, hbmMemOld;			// ������۸��� ���Ͽ�!
	static RECT rt;

	Location player_move;

	switch (iMessage) {
	case WM_CREATE:
		srand((unsigned int)time(NULL));

		cpy_hwnd = hwnd;

		GetClientRect(hwnd, &rt);

		mapimg.Load("Image\\Map\\Background.png");
		PLAYER[0].player_img.Load("Image\\Player\\Player.png");
		init_Monster_Image();
		init_ui(ui);
		init_Monster_Bullet_Image();	// �̹����� �ʱ�ȭ ��Ų��.
		PLAYER[0].control = PLAYER_ME;

		// ���� �ִϸ��̼� ���� Ÿ�̸�
		SetTimer(cpy_hwnd, 1, 1024, NULL);	// 1�� Ÿ�̸Ӹ� 1�ʰ�(1024ms) �����δ�
		SetTimer(cpy_hwnd, 2, 100, NULL);	// 2�� Ÿ�̸Ӹ� 0.1�ʰ�(100ms) �����δ�
		SetTimer(cpy_hwnd, 3, 10, NULL);	// 3�� Ÿ�̸Ӹ� 0.1�ʰ�(100ms) �����δ�
		SetTimer(cpy_hwnd, 4, 1024, NULL);	// 4�� Ÿ�̸Ӹ� 1�ʰ�(1024ms) �����δ�
		SetTimer(cpy_hwnd, 5, 10, NULL);
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
			// ���� �ִϸ��̼� ��ȭ
			for (int i = 0; i < MONSTER_COUNT; ++i) {
				change_enemy_ani(0, i);
				change_enemy_ani(1, i);
				change_enemy_ani(2, i);
			}
			break;

		case 2:
			// ���� ��ġ ��ȭ
			for (int i = 0; i < MONSTER_COUNT; ++i)
			{
				change_enemy_location(0, i);
				change_enemy_location(1, i);
				change_enemy_location(2, i);
			}
			break;

		case 3:
			// ���� ��ġ ��ȭ
			for (int i = 0; i < MONSTER_COUNT; ++i)
			{
				change_enemy_bullet(0, i);
				change_enemy_bullet(1, i);
				change_enemy_bullet(2, i);
			}
			break;

		case 4:
			// ���� �Ѿ� �߰� ����
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
		draw_enemybullet(mem0dc);	// �Ѿ��� �׸���.
		
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

