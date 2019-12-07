#include "main.h"
#include "protocol.h"


CLIENT g_Clients[MAX_Player];
SOCKET listen_sock;
HANDLE tThread, cThread;

bool playGame = true;	// 두명 이상 들어왔는지 판단 여부

clock_t send_time;

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
	cThread = CreateThread(NULL, 0, Calc_Thread, NULL, 0, NULL);

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
		g_Clients[new_id].bullet.reserve(PLAYER_BULLET_LIMIT);		// 총알은 최대 100개까지만 생성 되게 제한
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
	{
		cs_packet_dir *my_packet = reinterpret_cast<cs_packet_dir *>(packet);
		if (my_packet->dir == VK_RIGHT) {
			if (g_Clients[ci].position.x < WndX - PLAYER_SIZE - 30)
				g_Clients[ci].position.x += 10;
		}

		if (my_packet->dir == VK_LEFT) {
			if (g_Clients[ci].position.x > 0)
				g_Clients[ci].position.x -= 10;
		}

		if (my_packet->dir == VK_DOWN) {
			if (g_Clients[ci].position.y < WndY - PLAYER_SIZE - 30)
				g_Clients[ci].position.y += 10;
		}

		if (my_packet->dir == VK_UP) {
			if (g_Clients[ci].position.y > 0)
				g_Clients[ci].position.y -= 10;
		}

		send_location_packet(ci, ci);	// 보낸 사람에게 자신의 위치를 보내준다.
		for (int i = 0; i < MAX_Player; ++i) {
			// 자신의 아이디를 제외한 다른 사람들에게 보낸 사람에 위치를 보내준다.
			if (i == ci)
				continue;
			send_location_packet(i, ci);
		}
	}
	break;


	case CS_PACKET_ATTACK:
	{
		// 공격 패킷을 받더라도, 2명이상 들어 왔을 경우에만 처리를 한다.
		// 0.2초에 한번씩 총알을 생성하게 수정함.
		if (playGame == true && GetTickCount() - g_Clients[ci].bullet_push_time >= 200) {

			// Vector의 총알 생성이 PLAYER_BULLET_LIMIT 크거나 같을 경우에는 더이상 생성 하지 않는다.
			if (g_Clients[ci].bullet.size() >= PLAYER_BULLET_LIMIT)
				break;
			
			cs_packet_attack *attack_packet = reinterpret_cast<cs_packet_attack *>(packet);
			printf("[%d] 총알 생성 %d\t", ci, g_Clients[ci].bullet.size());

			// 총알 생성을 위한 플레이어 위치 기록
			Location temp_location;
			temp_location.x = g_Clients[ci].position.x + 55;
			temp_location.y = g_Clients[ci].position.y + 20;

			// Vector에 넣을 총알 생성
			Bullet bullet;
			bullet.position = temp_location;		// 총알의 위치는 현재 플레이어의 위치를 넣어준다.
			bullet.type = 5;	// add_player_bullet 에서 type 5라고 적혀 있어서 5라고 넣음
			bullet.dir = 0; // 플레이어 객체의 총알 방향은 일자로 나간다면 빈값으로 넣는다.
			bullet.bullet_type = 0; // 총알 타입도 없을 경우 빈값으로 넣는다.
			bullet.draw = true;		// 화면에 그려줘야 하므로 true
			g_Clients[ci].bullet.emplace_back(bullet);	// temp로 만든 bullet를 vector에 넣어준다.
			g_Clients[ci].bullet_push_time = GetTickCount();	// 현재 시간을 넣어줘서 위에서 1초 뒤에 총알을 넣을 수 있게 방지를 한다.

			// 총알 발사 이후 클라에게 총알 발사 했다고 값을 전송
			for (int i = 0; i < MAX_Player; ++i) {
				// 2명이 들어온 이후 부터 총알 발사가 가능하므로, 2명이 접속해 있을 때만 처리를 한다.
				if (playGame == false)
					continue;
				// 접속해 있는 클라이언트 들에게 전송
				if (g_Clients[i].connect == true) {
					sc_packet_bullet packet;
					packet.type = SC_PACKET_BULLET;
					packet.no = ci;		// 총알 발사자가 누구인지
					packet.bullet = bullet;	// 해당 총알에 대한 값을 전송
					SendPacket(g_Clients[i].sock, reinterpret_cast<unsigned char *>(&packet), sizeof(packet));
				}

			}
			
		}
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
		int StartTime, bulletTime;
		StartTime = GetTickCount();
		bulletTime = GetTickCount();
		int progress_time = 0;
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
			if (new_id==-1) {
				// 2명이 들어왔을 경우 playGame을 True로 변경하여 다른 곳에서는 해당 변수로 2명이 들어온지 판단한다.
				playGame = true;

				// 서버 전체 시간 관련 타이머 처리 [1초 마다 처리를 한다.]
				if (GetTickCount() - StartTime >= 1000) {
					//1초가 지남
					//printf("%d초 경과\t", progress_time);
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

				// 총알 관련한 타이머 처리 [0.05초 마다 처리를 한다.]
				if (GetTickCount() - bulletTime >= 50) {
					bulletTime = GetTickCount();

					// 총알 발사 및 이동
					for (int i = 0; i < MAX_Player; ++i) {
						// 클라이언트가 접속을 했을 경우에만 처리
						if (g_Clients[i].connect == true) {
							//add_player_bullet(g_Clients);	// 이미 Attack Packet에서 생성을 해준다.
							
							add_bullet_position(i);
						}
					}
				}


			}
			else {
				 //2명이 들어오지 않았을 경우 false 처리 한다.
				playGame = false;
			}


		}
		return 0;
	}

	DWORD WINAPI Calc_Thread(void* parameter) {
		//while (1) {
		//	// 2명이 모두 들어왔을 경우
		//	if (playGame == true) {
		//		// 총알 발사 및 이동
		//		for (int i = 0; i < MAX_Player; ++i) {
		//			// 클라이언트가 접속을 했을 경우에만 처리
		//			if (g_Clients[i].connect == true) {
		//				//add_player_bullet(g_Clients);	// 이미 Attack Packet에서 생성을 해준다.
		//				add_bullet_position(i);
		//			}
		//		}
		//	}
		//}
		return 0;
	}

	void add_bullet_position(int ci) {
		if (g_Clients[ci].bullet.size() > 0) {
			// Vector 를 순회하며, 총알 위치를 이동 시켜준다.
			for (std::vector<Bullet>::iterator it = g_Clients[ci].bullet.begin(); it != g_Clients[ci].bullet.end(); ) {
				// 총알이 범위를 벗어날 경우 삭제 처리를 한다.
				if (it->position.x >= WndX) {
					it = g_Clients[ci].bullet.erase(it);
				}
				else {
					// 총알 범위 이동
					it->position.x += 10;
					it++;
				}
			}
		}
	}

	void add_player_bullet(CLIENT player[2]) {
		for (int i = 0; i < 2; ++i) {
			if (player[i].Attack == true) {
				Bullet tmp;
				tmp.position.x = player[i].position.x + 55;
				tmp.position.y = player[i].position.y + 20;
				tmp.type = 5;
				tmp.dir = 0;
				tmp.bullet_type = 0;
				player[i].bullet.emplace_back(tmp);
			}
		}
	}