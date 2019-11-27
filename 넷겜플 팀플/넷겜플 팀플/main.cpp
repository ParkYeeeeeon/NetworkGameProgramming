// �ܼ�â ����
#pragma comment(linker,"/entry:WinMainCRTStartup /subsystem:console")

#include <Windows.h>
#include <vector>
#include <atlImage.h>
#include <math.h>
#include <mmsystem.h>
#include "Utill.h"
#include "Enemy.h"
#include "Player.h"
#include "GameState.h"
#include "protocol.h"
#pragma comment(lib,"winmm.lib") 

using namespace std;

#define WndX 1080	
#define WndY 720

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
LPCTSTR lpszClass = "ApiBase";

SOCKET sock;
HANDLE hThread;

Player PLAYER[2];
UI ui;
CImage mapimg;

cs_packet_dir packet;


void crash_check();

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
	


	switch (iMessage) {
	case WM_CREATE:
		srand((unsigned int)time(NULL));
		init_sock();
		cpy_hwnd = hwnd;

		GetClientRect(hwnd, &rt);

		packet.type = CS_PACKET_DIR;
		packet.dirX = 0;
		packet.dirY = 0;

		set_player(PLAYER);
		mapimg.Load("Image\\Map\\Background.png");


		init_Monster_Image();
		init_ui(ui);
		init_Monster_Bullet_Image();	// �̹����� �ʱ�ȭ ��Ų��.

		// ���� �ִϸ��̼� ���� Ÿ�̸�
		SetTimer(cpy_hwnd, 1, 10000, NULL);	// 1�� Ÿ�̸Ӹ� 10�ʰ�(10000ms) �����δ�
		SetTimer(cpy_hwnd, 2, 100, NULL);	// 2�� Ÿ�̸Ӹ� 0.1�ʰ�(100ms) �����δ�
		SetTimer(cpy_hwnd, 3, 5000, NULL);	// 3�� Ÿ�̸Ӹ� 5�ʰ�(5000ms) �����δ�
		SetTimer(cpy_hwnd, 4, 2500, NULL);	// 4�� Ÿ�̸Ӹ� 2.5�ʰ�(2500) �����δ�

		SetTimer(cpy_hwnd, 5, 10, NULL);	// �÷��̾� �̵� Ÿ�̸�
		SetTimer(cpy_hwnd, 6, 100, NULL);	// �÷��̾� �Ѿ� Ÿ�̸�

		SetTimer(cpy_hwnd, 7, 66, NULL);
		break;

	

	case WM_CHAR:
		break;


	case WM_LBUTTONDOWN:
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_UP:
		{
			/*for (int i = 0; i < 2; ++i) {
				if (PLAYER[i].control == PLAYER_ME) {
					PLAYER[i].moveY = 2;
				}
			}*/
			//packet.type = CS_PACKET_DIR;
			packet.dirY = VK_DOWN_UP;
			//SendPacket(sock, reinterpret_cast<unsigned char *>(&packet), sizeof(packet));
		}
		break;

		case VK_DOWN:
		{
			/*for (int i = 0; i < 2; ++i) {
				if (PLAYER[i].control == PLAYER_ME) {
					PLAYER[i].moveY = 1;
				}
			}*/
			//packet.type = CS_PACKET_DIR;
			packet.dirY = VK_DOWN_DOWN;
			//SendPacket(sock, reinterpret_cast<unsigned char *>(&packet), sizeof(packet));
		}
		break;

		case VK_LEFT:
		{
			/*for (int i = 0; i < 2; ++i) {
				if (PLAYER[i].control == PLAYER_ME) {
					PLAYER[i].moveX = 2;
				}
			}*/
			//packet.type = CS_PACKET_DIR;
			packet.dirX = VK_DOWN_LEFT;
			//SendPacket(sock, reinterpret_cast<unsigned char *>(&packet), sizeof(packet));
		}
		break;

		case VK_RIGHT:
		{
			/*for (int i = 0; i < 2; ++i) {
				if (PLAYER[i].control == PLAYER_ME) {
					PLAYER[i].moveX = 1;
				}
			}*/
			//packet.type = CS_PACKET_DIR;
			packet.dirX = VK_DOWN_RIGHT;
			//SendPacket(sock, reinterpret_cast<unsigned char *>(&packet), sizeof(packet));
		}
		break;

		case VK_SPACE:
		{
			for (int i = 0; i < 2; ++i) {
				if (PLAYER[i].control == PLAYER_ME)
					PLAYER[i].fire = true;
			}
			//packet.type = CS_PACKET_DIR;
			//packet.dir = VK_DOWN_SPACE;
			//SendPacket(sock, reinterpret_cast<unsigned char *>(&packet), sizeof(packet));
		}
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
			/*for (int i = 0; i < 2; ++i) {
				if (PLAYER[i].control == PLAYER_ME) {
					PLAYER[i].moveY = 0;
				}
			}*/
			//packet.type = CS_PACKET_DIR;
			packet.dirY = VK_UP_UP;
			//SendPacket(sock, reinterpret_cast<unsigned char *>(&packet), sizeof(packet));
			break;
		case VK_DOWN:
			/*for (int i = 0; i < 2; ++i) {
				if (PLAYER[i].control == PLAYER_ME) {
					PLAYER[i].moveY = 0;
				}
			}*/
			//packet.type = CS_PACKET_DIR;
			packet.dirY = VK_UP_DOWN;
			//SendPacket(sock, reinterpret_cast<unsigned char *>(&packet), sizeof(packet));
			break;
		case VK_LEFT:
			/*for (int i = 0; i < 2; ++i) {
				if (PLAYER[i].control == PLAYER_ME) {
					PLAYER[i].moveX = 0;
				}
			}*/
			//packet.type = CS_PACKET_DIR;
			packet.dirX = VK_UP_LEFT;
			//SendPacket(sock, reinterpret_cast<unsigned char *>(&packet), sizeof(packet));
			break;
		case VK_RIGHT:
		/*	for (int i = 0; i < 2; ++i) {
				if (PLAYER[i].control == PLAYER_ME) {
					PLAYER[i].moveX = 0;
				}
			}*/
			//packet.type = CS_PACKET_DIR;
			packet.dirX = VK_UP_RIGHT;
			//SendPacket(sock, reinterpret_cast<unsigned char *>(&packet), sizeof(packet));
			break;
		case VK_SPACE:
			for (int i = 0; i < 2; ++i) {
				if (PLAYER[i].control == PLAYER_ME)
					PLAYER[i].fire = false;
			}
			//packet.type = CS_PACKET_DIR;
			//packet.dir = VK_UP_SPACE;
			//SendPacket(sock, reinterpret_cast<unsigned char *>(&packet), sizeof(packet));
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
			// ���� �ٽ� ��Ȱ
			revival_enemy();
			break;

		case 4:
			// ���� �Ѿ� �߰� ����
			for (int i = 0; i < MONSTER_COUNT; i++) {
				add_enemy_bullet(0, i);
				add_enemy_bullet(1, i);
				add_enemy_bullet(2, i);
			}
			break;

		case 5:
			for (int i = 0; i < 2; ++i) {
				if (PLAYER[i].control == PLAYER_ME) {
					switch (PLAYER[i].moveX) {
					case 1:
						PLAYER[i].position.x += 3;
						break;
					case 2:
						PLAYER[i].position.x -= 3;
						break;
					}
					switch (PLAYER[i].moveY) {
					case 1:
						PLAYER[i].position.y += 3;
						break;
					case 2:
						PLAYER[i].position.y -= 3;
						break;
					}
				}
			}

			add_bullet_position(PLAYER);

			crash_check();
			break;

		case 6:
			for (int i = 0; i < 2; ++i) {
				if (PLAYER[i].control == PLAYER_ME) {
					if (PLAYER[i].fire == true) {
						add_player_bullet(PLAYER);
						for (vector<Bullet>::iterator j = PLAYER[i].bullet.begin(); j < PLAYER[i].bullet.end();) {
							if (j->position.x > WndX)
								j = PLAYER[i].bullet.erase(j);
							else
								++j;
						}
					}

				}
			}

			break;

		case 7:
			printf("X : %d\n", packet.dirX);
			printf("Y : %d\n", packet.dirY);
			SendPacket(sock, reinterpret_cast<unsigned char *>(&packet), sizeof(packet));
			packet.dirX = 0;
			packet.dirY = 0;
			break;

		case 8:
			break;

		case 9:
			break;

		case 10:

			break;
		}
		InvalidateRect(hwnd, NULL, false);
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
		draw_player(mem0dc, PLAYER);
		draw_playerbullet(mem0dc, PLAYER);
		draw_enemybullet(mem0dc);	// �Ѿ��� �׸���.
		draw_bullet_status(mem0dc);
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


void crash_check() {
	for (int i = 0; i < 2; ++i) {
		if (PLAYER[i].control == PLAYER_ME) {
			Location player_center = PLAYER[i].position;
			player_center.x += PLAYER_SIZE / 2;
			player_center.y += PLAYER_SIZE / 2;

			for (vector<Bullet>::iterator j = bullet.begin(); j < bullet.end();) {
				Location bullet_center = j->position;
				bullet_center.x += MONSTER_BULLET_SIZE / 2;
				bullet_center.y += MONSTER_BULLET_SIZE / 2;
				if ((PLAYER_SIZE / 2) + (MONSTER_BULLET_SIZE / 2) > get_distance(player_center, bullet_center)) {
					j = bullet.erase(j);
					PLAYER[i].hp--;
					if (ui.hp[i] != 0)
						ui.hp[i]--;
				}
				else
					++j;
			}
		}
	}
}

void init_sock() {
	// Ŭ���̾�Ʈ Sock ����
	int retval;

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		err_quit((char *)"connect()");

	// socket()
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) err_quit((char *)"socket()");

	// connect()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);
	bool flag = TRUE;
	setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(flag));
	retval = connect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
		err_quit((char *)"connect()");

	hThread = CreateThread(NULL, 0, Read_Thread, (LPVOID)sock, 0, NULL);
	if (hThread == NULL) {
		closesocket(sock);
	}
	else {
		CloseHandle(hThread);
	}
}

DWORD WINAPI Read_Thread(LPVOID arg) {
	int len;						// ������ ũ��

	SOCKET client_sock = (SOCKET)arg;
	SOCKADDR_IN clientaddr;

	// Ŭ���̾�Ʈ ���� ���
	int addrlen = sizeof(clientaddr);
	getpeername(client_sock, (SOCKADDR *)&clientaddr, &addrlen);

	while (1) {
		int retval = recvn(client_sock, (char *)&len, sizeof(int), 0); // ������ �ޱ�(���� ����)
		if (retval == SOCKET_ERROR) {
			err_display((char *)"recv()");
			break;
		}

		if (len >= 0) { // ���� ���̰� �������� ��츸 ���� ���̸� �޴´�.
			//printf("Packet Size : %d\n", len);

			char *buf = new char[len];
			retval = recvn(client_sock, buf, len, 0); // ������ �ޱ�(���� ����)
			//retval = recv( client_sock, buf, len, 0 );
			if (retval == SOCKET_ERROR) {
				err_display((char *)"recv()");
				break;
			}
			//buf[retval] = '\0';
			//printf("Packet 0��° : %d\n", buf[0]);
			ProcessPacket(0, buf);
			
		}


	}
	return 0;
}

void ProcessPacket(int ci, char *packet) {
	switch (packet[0]) {
	case SC_PACKET_CINO:
		//printf("SC_PACKET_CINO\n");
		sc_packet_cino *my_packet;
		my_packet = reinterpret_cast<sc_packet_cino *>(packet);
		printf("Client User No : %d\n", my_packet->no);
		for (int i = 0; i < 2; ++i) {
			if (i == my_packet->no)
				PLAYER[i].control = PLAYER_ME;
			else
				PLAYER[i].control = PLAYER_OTHER;
			printf("%d\n", PLAYER[i].control);
		}
		break;

	case SC_PACKET_PLAYER_0:
		cs_packet_player *player_0;
		player_0 = reinterpret_cast<cs_packet_player *>(packet);
		PLAYER[0].position = player_0->position;
		PLAYER[0].hp = player_0->hp;
		PLAYER[0].bullet_damage = player_0->bullet_damage;
		PLAYER[0].attack_speed = player_0->attack_speed;
		PLAYER[0].bomb = player_0->bomb;
		//PLAYER[0].bullet = player_0->b;
		break;

	case SC_PACKET_PLAYER_1:
		cs_packet_player *player_1;
		player_1 = reinterpret_cast<cs_packet_player *>(packet);
		PLAYER[1].position = player_1->position;
		PLAYER[1].hp = player_1->hp;
		PLAYER[1].bullet_damage = player_1->bullet_damage;
		PLAYER[1].attack_speed = player_1->attack_speed;
		PLAYER[1].bomb = player_1->bomb;
		//PLAYER[0].bullet = player_1->b;
		break;

	}
}