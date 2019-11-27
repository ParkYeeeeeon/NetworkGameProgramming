#include "main.h"
#include "protocol.h"


CLIENT g_Clients[MAX_Player];
SOCKET listen_sock;

int new_id = -1;	// 몇 번째 클라이언트 인지 확인 하기 위한 변수
clock_t send_time;

Player PLAYER[2];
UI ui;


int main() {
	init();

	std::thread accept_thread{ Accept_Thread };
	accept_thread.join();
}

void init() {
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);
	listen_sock = socket(AF_INET, SOCK_STREAM, 0);

	if (listen_sock == INVALID_SOCKET) err_quit("socket()");

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	bool flag = TRUE;
	setsockopt(listen_sock, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(flag));

	int retval = bind(listen_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");

	for (int i = 0; i < MAX_Player; ++i)
		g_Clients[i].connect = false;

	printf("Server init Complete..!\n");
}

void Accept_Thread() {
	SOCKADDR_IN clientaddr;
	HANDLE hThread, CalcThread;
	while (1) {
		// accept()
		int addrlen = sizeof(clientaddr);
		SOCKET client_sock = accept(listen_sock, (SOCKADDR *)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}
		printf("Client IP : %s, Port : %d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		//------------------------------------------------------------------------------------------

		for (int i = 0; i < MAX_Player; ++i) {
			if (g_Clients[i].connect == false) {
				new_id = i;
				break;
			}
		}
		//------------------------------------------------------------------------------------------
		if (-1 == new_id) {
			printf("설정된 인원 이상으로 접속하여 차단하였습니다..!\n");
			closesocket(client_sock);
			continue;
		}
		//------------------------------------------------------------------------------------------
		// 캐릭터 초기화.
		g_Clients[new_id].cliend_id = new_id;
		g_Clients[new_id].connect = true;
		//------------------------------------------------------------------------------------------
		// 클라이언트 고유번호 보내기
		sc_packet_cino packet;
		packet.type = SC_PACKET_CINO;
		packet.no = new_id;
		SendPacket(client_sock, reinterpret_cast<unsigned char *>(&packet), sizeof(packet));
		printf("Client No : %d\n", new_id);
		//------------------------------------------------------------------------------------------
		// Work_Thread 시작
		Thread_Parameter* params = new Thread_Parameter;
		params->sock = client_sock;
		params->ci = new_id;

		if (new_id == 1) {
			CalcThread = CreateThread(NULL, 0, Calc_Thread, NULL, 0, NULL);
			send_time = clock();
		}

		hThread = CreateThread(NULL, 0, Work_Thread, params, 0, NULL);
		if (hThread == NULL) {
			closesocket(client_sock);
		}
		else {
			CloseHandle(hThread);
		}
		//------------------------------------------------------------------------------------------
	}
	closesocket(listen_sock);
	// 윈속 종료
	WSACleanup();
}

void DisconnectClient(int ci) {
	printf("Client ID : %d Disconnect\n", ci);
	g_Clients[ci].connect = false;
}

DWORD WINAPI Work_Thread(void* parameter) {
	int len;						// 데이터 크기

	clock_t now;

	Thread_Parameter* params = (Thread_Parameter*)parameter;

	printf("Thread Client ID : %d\n", params->ci);
	SOCKET client_sock = params->sock;
	SOCKADDR_IN clientaddr;

	// 클라이언트 정보 얻기
	int addrlen = sizeof(clientaddr);
	getpeername(client_sock, (SOCKADDR *)&clientaddr, &addrlen);


	while (1) {
		//-----------------------------------------------------------------------------------------------------------
		// 플레이어 이동 좌표 전송
		if (new_id == 1) {
			now = clock();
			if (difftime(now, send_time) >= (clock_t)66) {
				cs_packet_player packet;
				for (int i = 0; i < MAX_Player; ++i) {
					if (i == 0) {
						packet.type = SC_PACKET_PLAYER_0;
					}
					else
						packet.type = SC_PACKET_PLAYER_1;
					packet.position = PLAYER[i].position;
					packet.hp = PLAYER[i].hp;
					packet.bullet_damage = PLAYER[i].bullet_damage;
					packet.attack_speed = PLAYER[i].attack_speed;
					packet.bomb = PLAYER[i].bomb;
					//packet.b = PLAYER[i].bullet;
					SendPacket(client_sock, reinterpret_cast<unsigned char *>(&packet), sizeof(packet));
					printf("[%d] 위치 보냄 x : %d, y = %d\n", packet.type, packet.position.x, packet.position.y);
				}

				send_time = now;
			}
		}
		//-------------------------------------------------------------------------------------------------------------
		int retval = recvn(client_sock, (char *)&len, sizeof(int), 0); // 데이터 받기(고정 길이)
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			DisconnectClient(params->ci);
			closesocket(client_sock);
			break;
		}

		if (len >= 0) { // 고정 길이가 정상적일 경우만 가변 길이를 받는다.
			//printf( "Packet Size : %d\n", len );

			char *buf = new char[len];
			retval = recvn(client_sock, buf, len, 0); // 데이터 받기(가변 길이)
			if (retval == SOCKET_ERROR) {
				err_display("recv()");
				DisconnectClient(params->ci);
				closesocket(client_sock);
				break;
			}
			//buf[retval] = '\0';
			//printf( "Packet 0번째 : %d\n", buf[0] );
			ProcessPacket(params->ci, buf);

		}



	}
	return 0;
}

void ProcessPacket(int ci, char *packet) {
	switch (packet[0]) {
	case CS_PACKET_DIR:
		cs_packet_dir *my_packet = reinterpret_cast<cs_packet_dir *>(packet);
		printf("[%d] : %d\n", ci, my_packet->dirX);
		printf("[%d] : %d\n", ci, my_packet->dirY);
		switch (my_packet->dirX)
		{
		case VK_UP_LEFT:
			PLAYER[ci].moveX = 0;
			break;
		case VK_UP_RIGHT:
			PLAYER[ci].moveX = 0;
			break;
		case VK_DOWN_LEFT:
			PLAYER[ci].moveX = 2;
			break;
		case VK_DOWN_RIGHT:
			PLAYER[ci].moveX = 1;
			break;
		}
		switch (my_packet->dirY)
		{
		case VK_UP_UP:
			PLAYER[ci].moveY = 0;
			break;
		case VK_UP_DOWN:
			PLAYER[ci].moveY = 0;
			break;
		case VK_DOWN_UP:
			PLAYER[ci].moveY = 2;
			break;
		case VK_DOWN_DOWN:
			PLAYER[ci].moveY = 1;
			break;
		}
		break;
	}
}

		

DWORD WINAPI Calc_Thread(LPVOID arg) {

	clock_t player_move_timer, now;
	player_move_timer = clock();

	set_player(PLAYER);

	while (1) {
		now = clock();
		if (difftime(now, player_move_timer) >= (clock_t)10) {	// 타이머 구현
			// 플레이어 이동
			for (int i = 0; i < MAX_Player; ++i) {
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

			player_move_timer = now;
		}


	}


	return 0;
}