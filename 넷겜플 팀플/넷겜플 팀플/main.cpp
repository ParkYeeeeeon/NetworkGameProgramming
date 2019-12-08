// 콘솔창 띄우기
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
CImage num_image[11]; // 숫자 이미지
int client_no = -1;	// 클라이언트 고유 번호 [서버에서 내려주는 고유 번호]
BOOL KeyBuffer[256]{ 0 };
int num; // 숫자 표시를 위한 배열

int total_timer = 0;
bool playGame = false;	// 두명 이상 들어왔는지 판단 여부

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
		init_sock();
		cpy_hwnd = hwnd;

		GetClientRect(hwnd, &rt);

		set_player(PLAYER);
		set_number();
		mapimg.Load("Image\\Map\\Background.png");


		init_Monster_Image();
		init_ui(ui);
		init_Monster_Bullet_Image();	// 이미지를 초기화 시킨다.

		// 몬스터 애니메이션 변경 타이머
		SetTimer(cpy_hwnd, 1, 10000, NULL);	// 1번 타이머를 10초간(10000ms) 움직인다
		SetTimer(cpy_hwnd, 2, 100, NULL);	// 2번 타이머를 0.1초간(100ms) 움직인다
		SetTimer(cpy_hwnd, 3, 1, NULL);	// 3번 타이머를 0.001초간(1ms) 움직인다
		SetTimer(cpy_hwnd, 4, 1000, NULL);	// 4번 타이머를 1초간(1000ms) 움직인다

		SetTimer(cpy_hwnd, 5, 10, NULL);	// 플레이어 충돌 체크
		SetTimer(cpy_hwnd, 6, 50, NULL);	// 플레이어 총알 타이머

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
			// 몬스터 애니메이션 변화
			if (playGame == true) {
				for (int i = 0; i < MONSTER_COUNT; ++i) {
					change_enemy_ani(0, i);
					change_enemy_ani(1, i);
					change_enemy_ani(2, i);
				}
			}
			break;

		case 2:
			// 몬스터 위치 변화
			if (playGame == true) {
				for (int i = 0; i < MONSTER_COUNT; ++i)
				{
					change_enemy_location(monster[i].kind, i);
				}
			}
			break;

		case 3:
			// 총알 움직임 처리
			move_enemybullet();
			break;

		case 4:
		{
			//--------------------------------------------------------------------------
			// 모든 플레이어가 들어왔는지 확인 처리
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
				// 플레이어가 접속 해 있을 경우에만 처리를 한다.
				if (PLAYER[i].connect == true) {

					// Vector 를 순회하며, 총알 위치를 이동 시켜준다.
					for (std::vector<Bullet>::iterator it = PLAYER[i].bullet.begin(); it != PLAYER[i].bullet.end(); ) {
						// 총알이 범위를 벗어날 경우 삭제 처리를 한다.
						if (it->position.x >= WndX) {
							it = PLAYER[i].bullet.erase(it);
						}
						else {
							// 총알 범위 이동
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

		// 플레이어가 모두 들어 왔을 경우에만 몬스터를 그려준다.
		if (playGame == true) {
			draw_enemy(mem0dc);
			draw_enemybullet(mem0dc);	// 총알을 그린다.
		}
		for (int i = 0; i < LIMIT_PLAYER; ++i) {
			// 플레이어가 접속 했을 때만 그려 준다.
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
	// 클라이언트 Sock 시작
	int retval;

	// 윈속 초기화
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

	// 클라이언트 정보 얻기
	int addrlen = sizeof(clientaddr);
	getpeername(client_sock, (SOCKADDR *)&clientaddr, &addrlen);

	while (1) {
		int len; // 데이터 크기
		int retval = recvn(client_sock, (char *)&len, sizeof(int), 0); // 데이터 받기(고정 길이)
		//int retval = recv(client_sock, (char *)&len, sizeof(int), 0);
		if (retval == SOCKET_ERROR) {
			err_display((char *)"recv()");
			reconnect_socket(client_sock);
			break;
		}
		//printf("Packet Size : %d\n", len);

		char buf[MAX_BUFSIZE]{ 0 };
		retval = recvn(client_sock, buf, len, 0); // 데이터 받기(가변 길이)
		//retval = recv(client_sock, buf, len, 0);
		if (retval == SOCKET_ERROR) {
			err_display((char *)"recv()");
			reconnect_socket(client_sock);
			break;
		}
		buf[retval] = '\0';

		if ((int)buf[0] >= LIMIT_PACKET_CLIENT_NO || (int)buf[0] <= 0) {
			
			// 패킷이 정상적으로 들어오지 못할 경우 서버와 Reconnect 처리를 한다.
			reconnect_socket(client_sock);
			break;

			// 고정 길이가 정상적일 경우만 가변 길이를 받는다.
			// 종종 서버에서 패킷이 이상하게 들어 올 가능성 때문에, 특정 패킷 값 이상은 안받는 처리를 한다.
			// 패킷 숫자가 늘어나면 해당 숫자도 늘려준다.
			/*printf("	Packet 동기화 오류...\n");
			printf("Packet 동기화 오류 0번째 : %d\n", buf[0]);
			char new_buf[MAX_BUFSIZE]{ 0 };
			for (int i = 4; i <= len; ++i) {
				new_buf[i - 4] = buf[i];
			}
			printf("	Packet 수정...\n");
			printf("Packet 수정 0번째 : %d\n", new_buf[0]);
			if ((int)new_buf[0] >= 15 || (int)new_buf[0] <= 0) {
				printf("Packet 수정으로도 실패 서버와 다시 연결..!\n");

				
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
		//printf("%d초 경과\t", my_packet->progress_time);
	}
	break;

	case SC_PACKET_BULLET:
	{
		// 서버에서 해당 플레이어의 총알 백터를 해당 플레이어에게 데이터를 넣어준다.
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
	// 숫자 그리기
	SetTextColor(hdc, RGB(0, 0, 255));
	SetBkMode(hdc, TRANSPARENT);

	num_image[num].Draw(hdc, x, y, 50, 50);
}

void draw_Timer(HDC hdc, int time) {

	int hour = time / 3600;
	time %= 3600;
	int minute = time / 60;
	time %= 60;

	// 분 단위가 10을 넘어 갔을 경우
	if (minute >= 10) {
		draw_number(hdc, minute / 10, 400, 0);
		draw_number(hdc, minute % 10, 450, 0);
	}
	else {
		draw_number(hdc, 0, 400, 0);
		draw_number(hdc, minute, 450, 0);
	}

	draw_number(hdc, 10, 500, 0);

	// 초 단위가 10을 넘어갔을 경우
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

	// 서버에다가 끊으라고 패킷을 전송 한다.
	// cs_packet_dir 을 적은 이유는 결국 type값을 기반으로 처리를 하므로, 아무거나 적은 것 이다.
	cs_packet_dir packet;
	packet.type = CS_PACKET_RECONNECT;
	packet.dir = CS_PACKET_RECONNECT;
	SendPacket(sock, reinterpret_cast<unsigned char *>(&packet), sizeof(packet));

	// 클라이언트 no를 초기화
	client_no = -1;

	// 스레드를 종료
	CloseHandle(hThread);

	// 소켓을 종료
	closesocket(sock);

	// 소켓을 다시 연결 한다.
	init_sock();
}