#include "main.h"
#include "protocol.h"


CLIENT g_Clients[MAX_Player];
SOCKET listen_sock;
HANDLE tThread;

clock_t send_time;

//Player PLAYER[2];
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

	tThread = CreateThread(NULL, 0, Timer_Thread, NULL, 0, NULL);

	printf("Server init Complete..!\n");
}

void Accept_Thread() {
	SOCKADDR_IN clientaddr;
	HANDLE hThread;
	while (1) {
		// accept()
		int new_id = -1;	// 몇 번째 클라이언트 인지 확인 하기 위한 변수
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
		g_Clients[new_id].sock = client_sock;
		g_Clients[new_id].position.x = 0;
		if (new_id % 2 == 0) {
			g_Clients[new_id].position.y = 100;
		}
		else {
			g_Clients[new_id].position.y = 400;
		}
		//------------------------------------------------------------------------------------------
		// 클라이언트 고유번호 보내기
		sc_packet_cino packet;
		packet.type = SC_PACKET_CINO;
		packet.no = new_id;
		SendPacket(client_sock, reinterpret_cast<unsigned char *>(&packet), sizeof(packet));
		printf("Client No : %d\n", new_id);
		//------------------------------------------------------------------------------------------
		// 이미 들어온 유저에게 접속 상태를 알린다.
		for (int i = 0; i < MAX_Player; ++i) {
			// 자신의 경우 CINO로 접속 여부를 보내주므로 패스 한다.
			if (i == new_id)
				continue;
			// 연결이 되어 있지 않는 클라이언트에게는 보내지 않는다.
			if (g_Clients[i].connect == false)
				continue;
			connect_player(i, new_id, true);
		}
		//------------------------------------------------------------------------------------------
		// 새로 들어온 유저가 이미 들어온 유저들의 상태를 받는다.
		for (int i = 0; i < MAX_Player; ++i) {
			// 자신의 경우 CINO로 접속 여부를 보내주므로 패스 한다.
			if (i == new_id)
				continue;
			// 연결이 되어 있지 않는 클라이언트에게는 보내지 않는다.
			if (g_Clients[i].connect == false)
				continue;
			connect_player(new_id, i, true);
		}
		//------------------------------------------------------------------------------------------
		// Work_Thread 시작
		Thread_Parameter* params = new Thread_Parameter;
		params->sock = client_sock;
		params->ci = new_id;

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

	// 상대가 나갔을 경우 나갔다고 모든 유저에게 전송을 해준다.
	for (int i = 0; i < MAX_Player; ++i) {
		// 자신의 경우 CINO로 접속 여부를 보내주므로 패스 한다.
		if (i == ci)
			continue;
		// 연결이 되어 있지 않는 클라이언트에게는 보내지 않는다.
		if (g_Clients[i].connect == false)
			continue;
		connect_player(i, ci, false);
	}
}

DWORD WINAPI Work_Thread(void* parameter) {
	int len;						// 데이터 크기

	
	Thread_Parameter* params = (Thread_Parameter*)parameter;

	printf("Thread Client ID : %d\n", params->ci);
	SOCKET client_sock = params->sock;
	SOCKADDR_IN clientaddr;

	// 클라이언트 정보 얻기
	int addrlen = sizeof(clientaddr);
	getpeername(client_sock, (SOCKADDR *)&clientaddr, &addrlen);


	while (1) {
		int retval = recvn(client_sock, (char *)&len, sizeof(int), 0); // 데이터 받기(고정 길이)
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			DisconnectClient(params->ci);
			closesocket(client_sock);
			break;
		}

		if (len > 0) { // 고정 길이가 정상적일 경우만 가변 길이를 받는다.
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

		if (my_packet->dir == VK_RIGHT) {
			g_Clients[ci].position.x += 5;
		}

		if (my_packet->dir == VK_LEFT) {
			g_Clients[ci].position.x -= 5;
		}

		if (my_packet->dir == VK_DOWN) {
			g_Clients[ci].position.y += 5;
		}

		if (my_packet->dir == VK_UP) {
			g_Clients[ci].position.y -= 5;
		}

		send_location_packet(ci, ci);	// 보낸 사람에게 자신의 위치를 보내준다.
		for (int i = 0; i < MAX_Player; ++i) {
			// 자신의 아이디를 제외한 다른 사람들에게 보낸 사람에 위치를 보내준다.
			if (i == ci)
				continue;
			send_location_packet(i, ci);
		}
		break;

	}
}

void send_location_packet(int me, int you) {
	// 플레이어가 접속을 했을 경우에만 패킷을 보낸다.
	if (g_Clients[me].connect == true) {
		sc_packet_location packet;
		packet.type = SC_PACKET_DIR;
		if (me != you) {
			// 상대 위치를 보낸다. 자신한테 보낸다.
			packet.ci = you;
			packet.x = g_Clients[you].position.x;
			packet.y = g_Clients[you].position.y;
			SendPacket(g_Clients[me].sock, reinterpret_cast<unsigned char *>(&packet), sizeof(packet));
		}
		else {
			// 자기 자신의 위치를 보낸다.
			packet.ci = me;
			packet.x = g_Clients[me].position.x;
			packet.y = g_Clients[me].position.y;
			SendPacket(g_Clients[me].sock, reinterpret_cast<unsigned char *>(&packet), sizeof(packet));
		}
	}
}

// me = 받는 플레이어, you는 나간 플레이어, value는 값
void connect_player(int me, int you, bool value) {
	sc_packet_connect packet;
	packet.type = SC_PACKET_CONNECT;
	packet.no = you;
	packet.connect = value;
	SendPacket(g_Clients[me].sock, reinterpret_cast<unsigned char *>(&packet), sizeof(packet));
}

DWORD WINAPI Timer_Thread(void* parameter) {
	int StartTime;
	StartTime = GetTickCount();
	int progress_time=0;
	while (1) {
		// 2명이 들어 왔을 경우 시작
		int new_id = -1;
		for (int i = 0; i < MAX_Player; ++i) {
			if (g_Clients[i].connect == false) {
				new_id = i;
				break;
			}
		}

		// 2명이 모두 들어왔을 경우
		if (-1 == new_id) {
			if (GetTickCount() - StartTime >= 1000) {
				//1초가 지남
				printf("%d초 경과\t", progress_time);
				StartTime = GetTickCount();
				progress_time += 1;
				for (int i = 0; i < MAX_Player; ++i) {
					sc_packet_time packet;
					packet.type = SC_PACKET_TIME;
					packet.progress_time = progress_time;
					if (g_Clients[i].connect == true)
						SendPacket(g_Clients[i].sock, reinterpret_cast<unsigned char *>(&packet), sizeof(packet));
				}

			}
		}
		

	}
	return 0;
}
