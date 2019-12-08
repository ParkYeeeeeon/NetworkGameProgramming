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
CImage num_image[11]; // ���� �̹���
int client_no = -1;	// Ŭ���̾�Ʈ ���� ��ȣ [�������� �����ִ� ���� ��ȣ]
BOOL KeyBuffer[256]{ 0 };
int num; // ���� ǥ�ø� ���� �迭

int total_timer = 0;
bool playGame = false;	// �θ� �̻� ���Դ��� �Ǵ� ����

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

		set_player(PLAYER);
		set_number();
		mapimg.Load("Image\\Map\\Background.png");


		init_Monster_Image();
		init_ui(ui);
		init_Monster_Bullet_Image();	// �̹����� �ʱ�ȭ ��Ų��.

		// ���� �ִϸ��̼� ���� Ÿ�̸�
		SetTimer(cpy_hwnd, 1, 10000, NULL);	// 1�� Ÿ�̸Ӹ� 10�ʰ�(10000ms) �����δ�
		SetTimer(cpy_hwnd, 2, 100, NULL);	// 2�� Ÿ�̸Ӹ� 0.1�ʰ�(100ms) �����δ�
		SetTimer(cpy_hwnd, 3, 1, NULL);	// 3�� Ÿ�̸Ӹ� 0.001�ʰ�(1ms) �����δ�
		SetTimer(cpy_hwnd, 4, 1000, NULL);	// 4�� Ÿ�̸Ӹ� 1�ʰ�(1000ms) �����δ�

		SetTimer(cpy_hwnd, 5, 10, NULL);	// �÷��̾� �浹 üũ
		SetTimer(cpy_hwnd, 6, 50, NULL);	// �÷��̾� �Ѿ� Ÿ�̸�

		SetTimer(cpy_hwnd, 7, 17, NULL);
		break;



	case WM_CHAR:
		break;


	case WM_LBUTTONDOWN:
		break;

	case WM_KEYDOWN:
		KeyBuffer[wParam] = TRUE;
		break;

	case WM_KEYUP:
		KeyBuffer[wParam] = FALSE;
		break;



	case WM_TIMER:
		switch (wParam)
		{
		case 1:
			// ���� �ִϸ��̼� ��ȭ
			if (playGame == true) {
				for (int i = 0; i < MONSTER_COUNT; ++i) {
					change_enemy_ani(0, i);
					change_enemy_ani(1, i);
					change_enemy_ani(2, i);
				}
			}
			break;

		case 2:
			// ���� ��ġ ��ȭ
			if (playGame == true) {
				for (int i = 0; i < MONSTER_COUNT; ++i)
				{
					change_enemy_location(monster[i].kind, i);
				}
			}
			break;

		case 3:
			// �Ѿ� ������ ó��
			move_enemybullet();
			break;

		case 4:
		{
			//--------------------------------------------------------------------------
			// ��� �÷��̾ ���Դ��� Ȯ�� ó��
			int new_id = -1;
			for (int i = 0; i < LIMIT_PLAYER; ++i) {
				if (PLAYER[i].connect == false) {
					new_id = i;
					break;
				}
			}

			if (-1 == new_id) {
				playGame = true;
			}
			else {
				playGame = false;
			}
			//--------------------------------------------------------------------------
		}
		break;

		case 5:
			crash_check();
			break;

		case 6:
			for (int i = 0; i < LIMIT_PLAYER; ++i) {
				// �÷��̾ ���� �� ���� ��쿡�� ó���� �Ѵ�.
				if (PLAYER[i].connect == true) {

					// Vector �� ��ȸ�ϸ�, �Ѿ� ��ġ�� �̵� �����ش�.
					for (std::vector<Bullet>::iterator it = PLAYER[i].bullet.begin(); it != PLAYER[i].bullet.end(); ) {
						// �Ѿ��� ������ ��� ��� ���� ó���� �Ѵ�.
						if (it->position.x >= WndX) {
							it = PLAYER[i].bullet.erase(it);
						}
						else {
							// �Ѿ� ���� �̵�
							it->position.x += 10;
							it++;
						}
					}
				}
			}
			break;

		case 7:
		{
			//printf("%d, %d, %d, %d\n", KeyBuffer[VK_LEFT], KeyBuffer[VK_RIGHT], KeyBuffer[VK_UP], KeyBuffer[VK_DOWN]);
			if (client_no != -1) {
				if (KeyBuffer[VK_LEFT] != 0)
				{
					cs_packet_dir packet;
					packet.type = CS_PACKET_DIR;
					packet.dir = VK_LEFT;
					SendPacket(sock, reinterpret_cast<unsigned char *>(&packet), sizeof(packet));
				}

				if (KeyBuffer[VK_RIGHT] != 0)
				{
					cs_packet_dir packet;
					packet.type = CS_PACKET_DIR;
					packet.dir = VK_RIGHT;
					SendPacket(sock, reinterpret_cast<unsigned char *>(&packet), sizeof(packet));
				}

				if (KeyBuffer[VK_UP] != 0)
				{
					cs_packet_dir packet;
					packet.type = CS_PACKET_DIR;
					packet.dir = VK_UP;
					SendPacket(sock, reinterpret_cast<unsigned char *>(&packet), sizeof(packet));
				}

				if (KeyBuffer[VK_DOWN] != 0)
				{
					cs_packet_dir packet;
					packet.type = CS_PACKET_DIR;
					packet.dir = VK_DOWN;
					SendPacket(sock, reinterpret_cast<unsigned char *>(&packet), sizeof(packet));
				}

				if (KeyBuffer[VK_SPACE] != 0) {
					cs_packet_attack packet;
					packet.type = CS_PACKET_ATTACK;
					packet.attack = true;
					packet.damage = 10;
					SendPacket(sock, reinterpret_cast<unsigned char *>(&packet), sizeof(packet));
				}

			}
		}
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

		// �÷��̾ ��� ��� ���� ��쿡�� ���͸� �׷��ش�.
		if (playGame == true) {
			draw_enemy(mem0dc);
			draw_enemybullet(mem0dc);	// �Ѿ��� �׸���.
		}
		for (int i = 0; i < LIMIT_PLAYER; ++i) {
			// �÷��̾ ���� ���� ���� �׷� �ش�.
			if (PLAYER[i].connect == true) {
				draw_player(mem0dc, PLAYER, i);
			}
		}
		draw_playerbullet(mem0dc, PLAYER);
		draw_bullet_status(mem0dc);
		draw_ui(mem0dc, ui);


		draw_Timer(mem0dc, total_timer);

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
	if (PLAYER[client_no].connect == true) {

		Location player_center = PLAYER[client_no].position;
		player_center.x += PLAYER_SIZE / 2;
		player_center.y += PLAYER_SIZE / 2;

		for (vector<Bullet>::iterator j = bullet.begin(); j < bullet.end();) {
			Location bullet_center = j->position;
			bullet_center.x += MONSTER_BULLET_SIZE / 2;
			bullet_center.y += MONSTER_BULLET_SIZE / 2;
			if ((PLAYER_SIZE / 2) + (MONSTER_BULLET_SIZE / 2) > get_distance(player_center, bullet_center)) {
				j = bullet.erase(j);
				PLAYER[client_no].hp--;
				if (ui.hp[client_no] != 0)
					ui.hp[client_no]--;
			}
			else
				++j;
		}

	}


	/*for (int i = 0; i < 2; ++i) {
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
	}*/
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
	bool flag = FALSE;
	setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(flag));

	int bsize = (MAX_BUFSIZE * 2);
	setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char*)&bsize, sizeof(bsize));
	setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char*)&bsize, sizeof(bsize));

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
	SOCKET client_sock = (SOCKET)arg;
	SOCKADDR_IN clientaddr;

	// Ŭ���̾�Ʈ ���� ���
	int addrlen = sizeof(clientaddr);
	getpeername(client_sock, (SOCKADDR *)&clientaddr, &addrlen);

	while (1) {
		int len; // ������ ũ��
		int retval = recvn(client_sock, (char *)&len, sizeof(int), 0); // ������ �ޱ�(���� ����)
		//int retval = recv(client_sock, (char *)&len, sizeof(int), 0);
		if (retval == SOCKET_ERROR) {
			err_display((char *)"recv()");
			reconnect_socket(client_sock);
			break;
		}
		//printf("Packet Size : %d\n", len);

		char buf[MAX_BUFSIZE]{ 0 };
		retval = recvn(client_sock, buf, len, 0); // ������ �ޱ�(���� ����)
		//retval = recv(client_sock, buf, len, 0);
		if (retval == SOCKET_ERROR) {
			err_display((char *)"recv()");
			reconnect_socket(client_sock);
			break;
		}
		buf[retval] = '\0';

		if ((int)buf[0] >= LIMIT_PACKET_CLIENT_NO || (int)buf[0] <= 0) {
			
			// ��Ŷ�� ���������� ������ ���� ��� ������ Reconnect ó���� �Ѵ�.
			reconnect_socket(client_sock);
			break;

			// ���� ���̰� �������� ��츸 ���� ���̸� �޴´�.
			// ���� �������� ��Ŷ�� �̻��ϰ� ��� �� ���ɼ� ������, Ư�� ��Ŷ �� �̻��� �ȹ޴� ó���� �Ѵ�.
			// ��Ŷ ���ڰ� �þ�� �ش� ���ڵ� �÷��ش�.
			/*printf("	Packet ����ȭ ����...\n");
			printf("Packet ����ȭ ���� 0��° : %d\n", buf[0]);
			char new_buf[MAX_BUFSIZE]{ 0 };
			for (int i = 4; i <= len; ++i) {
				new_buf[i - 4] = buf[i];
			}
			printf("	Packet ����...\n");
			printf("Packet ���� 0��° : %d\n", new_buf[0]);
			if ((int)new_buf[0] >= 15 || (int)new_buf[0] <= 0) {
				printf("Packet �������ε� ���� ������ �ٽ� ����..!\n");

				
			}
			else {
				ProcessPacket(client_no, new_buf);
			}*/
		}
		else {
			ProcessPacket(client_no, buf);
		}


	}
	return 0;
}

void ProcessPacket(int ci, char *packet) {
	switch (packet[0]) {
	case SC_PACKET_CINO:
	{
		sc_packet_cino *my_packet;
		my_packet = reinterpret_cast<sc_packet_cino *>(packet);
		printf("Client User No : %d\n", my_packet->no);
		PLAYER[my_packet->no].connect = true;
		client_no = my_packet->no;
	}
	break;

	case SC_PACKET_DIR:
	{
		sc_packet_location *my_packet;
		my_packet = reinterpret_cast<sc_packet_location *>(packet);
		//printf("[%d] %d, %d\n", my_packet->ci, my_packet->x, my_packet->y);
		PLAYER[my_packet->ci].position.x = my_packet->x;
		PLAYER[my_packet->ci].position.y = my_packet->y;
	}
	break;

	case SC_PACKET_CONNECT:
	{
		sc_packet_connect *my_packet;
		my_packet = reinterpret_cast<sc_packet_connect *>(packet);
		PLAYER[my_packet->no].connect = my_packet->connect;
	}
	break;

	case SC_PACKET_TIME:
	{
		sc_packet_time *my_packet;
		my_packet = reinterpret_cast<sc_packet_time *>(packet);
		total_timer = my_packet->progress_time;
		//printf("%d�� ���\t", my_packet->progress_time);
	}
	break;

	case SC_PACKET_BULLET:
	{
		// �������� �ش� �÷��̾��� �Ѿ� ���͸� �ش� �÷��̾�� �����͸� �־��ش�.
		sc_packet_bullet *my_packet;
		my_packet = reinterpret_cast<sc_packet_bullet *>(packet);
		PLAYER[my_packet->no].bullet.emplace_back(my_packet->bullet);
	}
	break;

	case SC_PACKET_MONSTER_LOCATION:
	{
		sc_packet_monster_location *my_packet;
		my_packet = reinterpret_cast<sc_packet_monster_location *>(packet);

		monster[my_packet->no].alive = my_packet->alive;
		monster[my_packet->no].ani = my_packet->ani;
		monster[my_packet->no].hp = my_packet->hp;
		monster[my_packet->no].kind = my_packet->kind;
		monster[my_packet->no].level = my_packet->level;
		monster[my_packet->no].position = my_packet->position;

	}
	break;

	case SC_PACKET_MONSTER_BULLET:
	{
		add_enemy_bullet();
	}
	break;


	}
}

void set_number() {
	char buffer[100] = { 0, };
	for (int i = 0; i < 11; i++)
	{
		sprintf(buffer, "Image\\UI\\num%d.png", i);
		num_image[i].Load(buffer);
	}

}

void draw_number(HDC hdc, int num, int x, int y) {
	// ���� �׸���
	SetTextColor(hdc, RGB(0, 0, 255));
	SetBkMode(hdc, TRANSPARENT);

	num_image[num].Draw(hdc, x, y, 50, 50);
}

void draw_Timer(HDC hdc, int time) {

	int hour = time / 3600;
	time %= 3600;
	int minute = time / 60;
	time %= 60;

	// �� ������ 10�� �Ѿ� ���� ���
	if (minute >= 10) {
		draw_number(hdc, minute / 10, 400, 0);
		draw_number(hdc, minute % 10, 450, 0);
	}
	else {
		draw_number(hdc, 0, 400, 0);
		draw_number(hdc, minute, 450, 0);
	}

	draw_number(hdc, 10, 500, 0);

	// �� ������ 10�� �Ѿ�� ���
	if (time >= 10) {
		draw_number(hdc, time / 10, 550, 0);
		draw_number(hdc, time % 10, 600, 0);
	}
	else {
		draw_number(hdc, 0, 550, 0);
		draw_number(hdc, time, 600, 0);
	}
}

void reconnect_socket(SOCKET &sock) {

	// �������ٰ� ������� ��Ŷ�� ���� �Ѵ�.
	// cs_packet_dir �� ���� ������ �ᱹ type���� ������� ó���� �ϹǷ�, �ƹ��ų� ���� �� �̴�.
	cs_packet_dir packet;
	packet.type = CS_PACKET_RECONNECT;
	packet.dir = CS_PACKET_RECONNECT;
	SendPacket(sock, reinterpret_cast<unsigned char *>(&packet), sizeof(packet));

	// Ŭ���̾�Ʈ no�� �ʱ�ȭ
	client_no = -1;

	// �����带 ����
	CloseHandle(hThread);

	// ������ ����
	closesocket(sock);

	// ������ �ٽ� ���� �Ѵ�.
	init_sock();
}