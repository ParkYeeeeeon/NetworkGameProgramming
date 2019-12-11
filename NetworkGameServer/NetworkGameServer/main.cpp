#include "main.h"
#include "protocol.h"


CLIENT g_Clients[MAX_Player];

Enemy monster[MONSTER_COUNT];
Enemy boss;
std::list<Bullet> bullet;
std::list<Bullet> bossBullet;
SOCKET listen_sock;
HANDLE tThread, cThread;

bool playGame = false;	// 두명 이상 들어왔는지 판단 여부

int main() {
	init();
	init_monster();
	init_boss();
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
	bool flag = FALSE;
	setsockopt(listen_sock, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(flag));

	int bsize = (MAX_BUFSIZE * 2);
	setsockopt(listen_sock, SOL_SOCKET, SO_SNDBUF, (char*)&bsize, sizeof(bsize));
	setsockopt(listen_sock, SOL_SOCKET, SO_RCVBUF, (char*)&bsize, sizeof(bsize));

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
		g_Clients[new_id].hp = 5;
		g_Clients[new_id].item = 3;
		g_Clients[new_id].ready = false;
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
			// 연결이 되어 있지 않는 클라이언트에게는 보내지 않는다.
			if (g_Clients[i].connect == false)
				continue;
			connect_player(i, new_id, true);
		}
		//------------------------------------------------------------------------------------------
		// 새로 들어온 유저가 이미 들어온 유저들의 상태를 받는다.
		for (int i = 0; i < MAX_Player; ++i) {
			// 연결이 되어 있지 않는 클라이언트에게는 보내지 않는다.
			if (g_Clients[i].connect == false)
				continue;
			connect_player(new_id, i, true);
		}

		//------------------------------------------------------------------------------------------
		// 클라이언트 몬스터 정보 보내기
		for (int mi = 0; mi < MONSTER_COUNT; mi++) {
			sc_packet_monster_location packet;
			packet.type = SC_PACKET_MONSTER_LOCATION;
			packet.no = mi;
			packet.alive = monster[mi].alive;
			packet.ani = monster[mi].ani;
			packet.hp = monster[mi].hp;
			packet.kind = monster[mi].kind;
			packet.level = monster[mi].level;
			packet.position = monster[mi].position;
			SendPacket(g_Clients[new_id].sock, reinterpret_cast<unsigned char *>(&packet), sizeof(packet));
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
	Thread_Parameter* params = (Thread_Parameter*)parameter;

	printf("Thread Client ID : %d\n", params->ci);
	SOCKET client_sock = params->sock;
	SOCKADDR_IN clientaddr;

	// 클라이언트 정보 얻기
	int addrlen = sizeof(clientaddr);
	getpeername(client_sock, (SOCKADDR *)&clientaddr, &addrlen);


	while (1) {
		//------------------------------------------------------------------------------------------
		int len;						// 데이터 크기
		int retval = recvn(client_sock, (char *)&len, sizeof(int), 0); // 데이터 받기(고정 길이)
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			DisconnectClient(params->ci);
			closesocket(client_sock);
			break;
		}

		char buf[MAX_BUFSIZE]{ 0 };
		retval = recvn(client_sock, buf, len, 0); // 데이터 받기(가변 길이)
		//retval = recv(client_sock, buf, len, 0);
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			DisconnectClient(params->ci);
			closesocket(client_sock);
			break;
		}
		buf[retval] = '\0';

		if ((int)buf[0] >= LIMIT_PACKET_CLIENT_NO || (int)buf[0] <= 0) {
			printf("[%d] 정상적이지 않은 패킷이 발송 되었습니다..!\n", params->ci);
		}
		else {
			ProcessPacket(params->ci, buf);
		}

	}
	return 0;
}

void ProcessPacket(int ci, char *packet) {
	switch (packet[0]) {

	case CS_PACKET_READY:
	{
		// 클라이언트에서 Ready 버튼을 눌렀을 경우 준비 완료된 상태로 바꾼다.
		g_Clients[ci].ready = true;
	}
	break;

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
			//printf("[%d] 총알 생성 %d\t", ci, g_Clients[ci].bullet.size());

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

	case CS_PACKET_RECONNECT:
	{
		DisconnectClient(ci);
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
	packet.hp = g_Clients[you].hp;
	packet.item = g_Clients[you].item;
	packet.connect = value;
	packet.ready = g_Clients[you].ready;
	packet.position.x = g_Clients[you].position.x;
	packet.position.y = g_Clients[you].position.y;
	SendPacket(g_Clients[me].sock, reinterpret_cast<unsigned char *>(&packet), sizeof(packet));
}

DWORD WINAPI Timer_Thread(void* parameter) {
	int StartTime, bulletTime, monsterTime, monsterSendTime, monsterBullet, monsterBulletMove, crashcheck_player_time;
	int crashcheck_monster_time, bossBulletTime, skill_check_time;
	StartTime = GetTickCount();
	bulletTime = GetTickCount();
	monsterTime = GetTickCount();
	monsterSendTime = GetTickCount();
	monsterBullet = GetTickCount();
	monsterBulletMove = GetTickCount();
	crashcheck_player_time = GetTickCount();
	crashcheck_monster_time = GetTickCount();
	bossBulletTime = GetTickCount();
	skill_check_time = GetTickCount();

	int progress_time = 0;
	int packet_type = 0;
	bool showWarning = false;
	bool showBoss = false;
	bool initPlayerReady = false;
	while (1) {

		// 2명이 들어온 상태에서 레디를 모두 완료 했을 경우 시작한다.
		int new_id = 0;
		for (int i = 0; i < MAX_Player; ++i) {
			if (g_Clients[i].connect == true && g_Clients[i].ready == true) {
				new_id++;
			}
		}

		// 2명이 모두 레디를 하였을 경우
		if (new_id == MAX_Player) {
			// 2명이 들어왔을 경우 playGame을 True로 변경하여 다른 곳에서는 해당 변수로 2명이 들어온지 판단한다.
			playGame = true;

			// 처음으로 둘다 레디를 하였을 경우
			if (initPlayerReady == false) {
				// 모든 접속된 플레이어에게 ready 정보를 알려준다.
				for (int i = 0; i < MAX_Player; ++i) {
					// 연결이 되어 있지 않는 클라이언트에게는 보내지 않는다.
					if (g_Clients[i].connect == false)
						continue;
					for (int j = 0; j < MAX_Player; ++j) {
						connect_player(i, j, true);
					}
				}
				initPlayerReady = true;
			}


			// 서버 전체 시간 관련 타이머 처리 [1초 마다 처리를 한다.]
			if (GetTickCount() - StartTime >= 1000) {
				//1초가 지남
				//printf("%d초 경과\t", progress_time);
				StartTime = GetTickCount();
				progress_time += 1;

				// 보스 출연시간 체크
				if (progress_time >= BOSS_COUNT && showWarning == false) {
					// 출연시간이 되면 시간을 0으로 초기화 하고, Warning를 보여 준다.
					progress_time = 0;
					showWarning = true;
					packet_type++;
				}
				else if (progress_time >= SHOW_WARNING_COUNT && showWarning == true && showBoss == false) {
					// Warning 시간이 끝난 경우 실제 보스가 나오게 처리를 한다.
					showBoss = true;
					packet_type++;
				}
				for (int i = 0; i < MAX_Player; ++i) {
					// 접속 하지 않은 경우 넘긴다.
					if (g_Clients[i].connect == false)
						continue;
					sc_packet_time packet;
					switch (packet_type) {
					case 0:
						packet.type = SC_PACKET_TIME;
						break;
					case 1:
						packet.type = SC_PACLKET_WARNING_START;
						break;
					case 2:
						packet.type = SC_PACLKET_BOSS_START;
						break;
					}
					packet.progress_time = progress_time;
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
						add_bullet_position(i);
					}
				}
			}

			// 몬스터, 보스 움직임 처리 [0.1초 마다 처리를 한다.]
			if (GetTickCount() - monsterTime >= 100) {
				monsterTime = GetTickCount();
				move_monster_location();
				// 보스 움직임 처리
				if (showBoss == true) {
					move_boss_location();
					if (boss.hp <= 0) {
						showBoss = false;
						showWarning = false;
						progress_time = 0;
						packet_type = 0;
						// 패킷 타입만 보내주는 처리 [보스 끝이라는 엔딩 타입만 보내준다.]
						sc_packet_bullet packet;
						packet.type = SC_PACKET_BOSS_END;
						for (int i = 0; i < MAX_Player; ++i) {
							if (g_Clients[i].connect == true) {
								SendPacket(g_Clients[i].sock, reinterpret_cast<unsigned char *>(&packet), sizeof(packet));
							}
						}
					}
				}
			}

			// 몬스터 움직임 동기화 처리 [5초 마다 처리를 한다.]
			if (GetTickCount() - monsterSendTime >= 5000) {
				monsterSendTime = GetTickCount();
				for (int i = 0; i < MAX_Player; ++i) {
					if (g_Clients[i].connect == true) {
						send_monster_location(i);
						if (showBoss == true) {
							send_boss_location(i);
						}
					}
				}
			}

			// 몬스터 총알, 살림 처리 [2초 마다 처리를 한다.]
			if (GetTickCount() - monsterBullet >= 2000) {
				monsterBullet = GetTickCount();
				add_enemy_bullet();
				revival_enemy();	// 2초마다 몬스터 다시 생성
			}

			// 보스  총알 [0.5초 마다 처리를 한다.]
			if (GetTickCount() - bossBulletTime >= 500) {
				bossBulletTime = GetTickCount();
				if (showBoss == true) {
					add_boss_bullet(progress_time);
				}
			}

			// 몬스터 총알 움직임 처리 [0.001초 마다 처리를 한다.]
			if (GetTickCount() - monsterBulletMove >= 1) {
				monsterBulletMove = GetTickCount();
				move_enemybullet();
			}

			// 적 -> 플레이어 충돌체크 [ 0.01초마다 처리를 한다. ]
			if (GetTickCount() - crashcheck_player_time >= 10) {
				crashcheck_player_time = GetTickCount();
				for (int i = 0; i < MAX_Player; ++i) {
					if (g_Clients[i].connect == true) {
						for (std::list<Bullet>::iterator b = bullet.begin(); b != bullet.end();)
						{
							if (b->draw == false) {
								++b;
								continue;
							}

							if (crash_check(g_Clients[i].position.x, g_Clients[i].position.y, b->position.x, b->position.y, b->bullet_type, false) == true) {
								// 충돌 처리
								b->draw = false;
								send_crash_player(i);
							}
							++b;
						}
						if (showBoss == true) {
							for (std::list<Bullet>::iterator b = bossBullet.begin(); b != bossBullet.end();)
							{
								if (b->draw == false) {
									++b;
									continue;
								}

								if (crash_check(g_Clients[i].position.x, g_Clients[i].position.y, b->position.x, b->position.y, b->bullet_type, false) == true) {
									// 충돌 처리
									b->draw = false;
									send_crash_player(i);
								}
								++b;
							}
						}
					}
				}
			}

			// 플레이어 -> 적 충돌체크 [ 0.01초마다 처리를 한다. ]
			if (GetTickCount() - crashcheck_monster_time >= 10) {
				crashcheck_monster_time = GetTickCount();

				for (int i = 0; i < MAX_Player; ++i) {
					if (g_Clients[i].connect == true) {
						for (std::list<Bullet>::iterator b = g_Clients[i].bullet.begin(); b != g_Clients[i].bullet.end();)
						{
							// 총알이 안그려지는 경우 넘긴다. [충돌체크 불필요]
							if (b->draw == false) {
								++b;
								continue;
							}
							for (int j = 0; j < MONSTER_COUNT; ++j) {
								if (monster[j].alive == false)
									continue;

								if (crash_check(monster[j].position.x, monster[j].position.y, b->position.x, b->position.y, 3, false) == true) {
									// 충돌 처리
									b->draw = false;
									monster[j].alive = false;
									send_crash_monster(i);
								}
							}

							if (showBoss == true) {
								if (crash_check(boss.position.x, boss.position.y, b->position.x, b->position.y, 3, true) == true) {
									// 충돌 처리
									b->draw = false;
									boss.hp -= 1;
								}
							}

							++b;
						}
					}
				}

			}


			// 플레이어 스킬 체크 [ 0.01초마다 체크 ]
			if (GetTickCount() - skill_check_time >= 10) {
				skill_check_time = GetTickCount();
				bool Prev = g_Clients[0].skill;

				if (skill_activation() == true) {
					for (int i = 0; i < MAX_Player; ++i) {
						g_Clients[i].skill = true;
					}
				}
				else {
					for (int i = 0; i < MAX_Player; ++i) {
						g_Clients[i].skill = false;
					}
				}
				if(Prev != g_Clients[0].skill)
					send_skill_activation();
			}

		}
		else {
			//2명이 들어오지 않았을 경우 false 처리 한다.
			playGame = false;
			initPlayerReady = false;
		}


	}
	return 0;
}

DWORD WINAPI Calc_Thread(void* parameter) {
	//while (1) {
	//	// Packet 보내는 처리를 진행한다. [0.001초 마다 처리를 한다.]
	//	// SendPacket으로 바로 처리를 할 경우 중간에 데이터가 꼬이는 현상이 나타난다.
	//	// 이를 해결 하기 위하여 Queue에 데이터를 삽입 후
	//	// 일정 시간마다 Queue값을 전송 하는 방식으로 수정
	//	SendPacketQueue();
	//}
	return 0;
}

void add_bullet_position(int ci) {
	if (g_Clients[ci].bullet.size() > 0) {
		// Vector 를 순회하며, 총알 위치를 이동 시켜준다.
		for (std::list<Bullet>::iterator it = g_Clients[ci].bullet.begin(); it != g_Clients[ci].bullet.end(); ) {
			// 총알이 범위를 벗어날 경우 삭제 처리를 한다.
			if (it->position.x >= WndX || it->draw == false) {
				it = g_Clients[ci].bullet.erase(it);
				continue;
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

void init_monster() {
	for (int i = 0; i < MONSTER_COUNT; i++) // 몬스터 위치 난수 생성
	{
		monster[i].position.x = (rand() % (display_end_x - display_start_x) + display_start_x) + 300;
		monster[i].position.y = rand() % (display_end_y - display_start_y) + display_start_y;
		monster[i].alive = true;
		monster[i].kind = i % 3;
		monster[i].hp = 100;
		monster[i].ani = 0;
		monster[i].level = 1;
	}
}

void init_boss() {
	boss.position.x = 750;
	boss.position.y = 200;
	boss.hp = 1000;
	boss.alive = true;
	boss.ani = 0;
	boss.kind = 0;
	boss.level = 1;
}

void move_monster_location() {
	for (int mi = 0; mi < MONSTER_COUNT; mi++) {
		// 몬스터 위치를 먼저 이동 시킨다.
		switch (monster[mi].kind) {
		case 0:
			monster[mi].position.x = monster[mi].position.x - 2;
			break;
		case 1:
			monster[mi].position.x = monster[mi].position.x - 3;
			break;
		case 2:
			monster[mi].position.x = monster[mi].position.x - 5;
			break;
		}

		if (monster[mi].position.x <= 0 || monster[mi].position.y <= 0 || monster[mi].position.y >= 720) {
			// 범위를 벗어나면 죽었다고 처리를 한다.
			monster[mi].alive = false;
		}
	}
}

void move_boss_location() {
	if (boss.ani == 0) {
		boss.position.y += 4;
	}
	else {
		boss.position.y -= 4;
	}
	if (boss.position.y >= 550) {
		boss.ani = 1;
	}
	else if (boss.position.y <= -100) {
		boss.ani = 0;
	}
}

void send_monster_location(int ci) {
	for (int mi = 0; mi < MONSTER_COUNT; mi++) {
		sc_packet_monster_location packet;
		packet.type = SC_PACKET_MONSTER_LOCATION;
		packet.no = mi;
		packet.alive = monster[mi].alive;
		packet.ani = monster[mi].ani;
		packet.hp = monster[mi].hp;
		packet.kind = monster[mi].kind;
		packet.level = monster[mi].level;
		packet.position = monster[mi].position;
		SendPacket(g_Clients[ci].sock, reinterpret_cast<unsigned char *>(&packet), sizeof(packet));
	}
}

void send_boss_location(int ci) {
	sc_packet_monster_location packet;
	packet.type = SC_PACKET_BOSS_LOCATION;
	packet.no = 0;
	packet.alive = boss.alive;
	packet.ani = boss.ani;
	packet.hp = boss.hp;
	packet.kind = boss.kind;
	packet.level = boss.level;
	packet.position = boss.position;
	SendPacket(g_Clients[ci].sock, reinterpret_cast<unsigned char *>(&packet), sizeof(packet));
}

void add_enemy_bullet() {
	for (int mi = 0; mi < MONSTER_COUNT; mi++) {
		// 몬스터가 살아 있을 경우에만 처리 한다.
		if (monster[mi].alive == false)
			continue;

		switch (monster[mi].kind) {
		case 0:
		{
			Bullet tmp_bullet;
			tmp_bullet.position.x = monster[mi].position.x - 20;
			tmp_bullet.position.y = monster[mi].position.y + 20;
			tmp_bullet.type = 0;
			tmp_bullet.dir = 0;
			tmp_bullet.bullet_type = 0;
			tmp_bullet.dir = 0;
			bullet.push_back(tmp_bullet);
		}
		break;

		case 1:
		{
			Bullet tmp_bullet;
			tmp_bullet.position.x = monster[mi].position.x - 20;
			tmp_bullet.position.y = monster[mi].position.y;
			tmp_bullet.type = 0;
			tmp_bullet.dir = 0;
			tmp_bullet.bullet_type = 1;
			tmp_bullet.dir = 0;
			bullet.push_back(tmp_bullet);

			tmp_bullet.position.x = monster[mi].position.x - 20;
			tmp_bullet.position.y = monster[mi].position.y + 40;
			tmp_bullet.type = 0;
			tmp_bullet.dir = 0;
			tmp_bullet.bullet_type = 1;
			tmp_bullet.dir = 0;
			bullet.push_back(tmp_bullet);
		}
		break;

		case 2:
		{
			for (int j = 0; j < 3; ++j) {
				Bullet tmp_bullet;
				tmp_bullet.position.x = monster[mi].position.x - 20;
				tmp_bullet.position.y = monster[mi].position.y + 20;
				tmp_bullet.type = 0;
				tmp_bullet.dir = 0;
				tmp_bullet.bullet_type = 0;
				tmp_bullet.dir = j;	// 직진 // 왼쪽 위로 // 왼쪽 아래
				bullet.push_back(tmp_bullet);
			}
		}
		break;

		}
	}

	// 총알을 발사 했다는 결과만 전송한다.
	// 서버, 클라 각각 계산을 처리한다.
	for (int i = 0; i < MAX_Player; ++i) {
		if (g_Clients[i].connect == true) {
			sc_packet_monster_bullet packet;
			packet.type = SC_PACKET_MONSTER_BULLET;
			packet.shoot = true;
			SendPacket(g_Clients[i].sock, reinterpret_cast<unsigned char *>(&packet), sizeof(packet));
		}
	}

}

void add_boss_bullet(int time) {
	boss.kind = time % 3;

	switch (boss.kind) {
	case 0:
	{
		Bullet tmp_bullet;
		tmp_bullet.position.x = boss.position.x - 20;
		tmp_bullet.position.y = boss.position.y + 20;
		tmp_bullet.type = 0;
		tmp_bullet.dir = 0;
		tmp_bullet.bullet_type = 0;
		tmp_bullet.dir = 0;
		bossBullet.push_back(tmp_bullet);
	}
	break;

	case 1:
	{
		Bullet tmp_bullet;
		tmp_bullet.position.x = boss.position.x - 20;
		tmp_bullet.position.y = boss.position.y;
		tmp_bullet.type = 0;
		tmp_bullet.dir = 0;
		tmp_bullet.bullet_type = 1;
		tmp_bullet.dir = 0;
		bossBullet.push_back(tmp_bullet);

		tmp_bullet.position.x = boss.position.x - 20;
		tmp_bullet.position.y = boss.position.y + 40;
		tmp_bullet.type = 0;
		tmp_bullet.dir = 0;
		tmp_bullet.bullet_type = 1;
		tmp_bullet.dir = 0;
		bossBullet.push_back(tmp_bullet);
	}
	break;

	case 2:
	{
		for (int j = 0; j < 3; ++j) {
			Bullet tmp_bullet;
			tmp_bullet.position.x = boss.position.x - 20;
			tmp_bullet.position.y = boss.position.y + 20;
			tmp_bullet.type = 0;
			tmp_bullet.dir = 0;
			tmp_bullet.bullet_type = 0;
			tmp_bullet.dir = j;	// 직진 // 왼쪽 위로 // 왼쪽 아래
			bossBullet.push_back(tmp_bullet);
		}
	}
	break;

	}

	// 총알을 발사 했다는 결과만 전송한다.
	// 서버, 클라 각각 계산을 처리한다.
	for (int i = 0; i < MAX_Player; ++i) {
		if (g_Clients[i].connect == true) {
			sc_packet_monster_bullet packet;
			packet.type = SC_PACKET_BOSS_BULLET;
			packet.shoot = boss.kind;
			SendPacket(g_Clients[i].sock, reinterpret_cast<unsigned char *>(&packet), sizeof(packet));
		}
	}

}

void move_enemybullet() {
	for (std::list<Bullet>::iterator i = bullet.begin(); i != bullet.end(); )
	{
		// 총알이 영역 밖으로 나갔을 경우 삭제
		if ((i->position.x <= 0 || i->position.y <= 0 || i->position.y >= 720) || i->draw == false) {
			// 범위를 벗어나면 삭제를 해준다.
			i = bullet.erase(i);
			continue;
		}
		else {
			change_enemy_bullet(i);
			++i;
		}
	}
}

void change_enemy_bullet(std::list<Bullet>::iterator i) {
	// 적 총알 그리기
		// 왼쪽 아래
	switch (i->dir) {
	case 0:
		// 직진
		i->position.x -= 3;
		break;
	case 1:
		// 왼쪽 위로
		i->position.x -= 3;
		i->position.y += 3;
		break;
	case 2:
		// 왼쪽 아래
		i->position.x -= 3;
		i->position.y -= 3;
		break;
	}
}

void revival_enemy() {
	// 몬스터가 죽었을 경우 랜덤한 위치에 다시 생성 시킨다.
	for (int mi = 0; mi < MONSTER_COUNT; mi++) {
		if (monster[mi].alive == false) {
			monster[mi].position.x = (rand() % (display_end_x - display_start_x) + display_start_x) + 300;
			monster[mi].position.y = rand() % (display_end_y - display_start_y) + display_start_y;
			monster[mi].alive = true;
			monster[mi].kind = mi % 3;
			monster[mi].hp = 100;
			monster[mi].ani = 0;
			monster[mi].level = 1;

			for (int i = 0; i < MAX_Player; ++i) {
				if (g_Clients[i].connect == true) {
					sc_packet_monster_location packet;
					packet.type = SC_PACKET_MONSTER_LOCATION;
					packet.no = mi;
					packet.alive = monster[mi].alive;
					packet.ani = monster[mi].ani;
					packet.hp = monster[mi].hp;
					packet.kind = monster[mi].kind;
					packet.level = monster[mi].level;
					packet.position = monster[mi].position;
					SendPacket(g_Clients[i].sock, reinterpret_cast<unsigned char *>(&packet), sizeof(packet));
				}
			}
		}
	}

}

bool crash_check(int myX, int myY, int uX, int uY, int uType, bool isBoss) {
	int mRight, mLeft, mBottom, mTop, uRight, uLeft, uBottom, uTop;
	// 플레이어 사각형 박스 만들기
	if (isBoss == false) {
		mRight = myX + 50;
		mLeft = myX;
		mBottom = myY + 50;
		mTop = myY;
	}
	else {
		mRight = myX + 452;
		mLeft = myX;
		mBottom = myY + 320;
		mTop = myY;
	}

	// 총알 사각형 박스 만들기
	switch (uType) {
	case 0:
		uRight = uX + 10;
		uBottom = uY + 10;
		break;
	case 1:
		uRight = uX + 20;
		uBottom = uY + 10;
		break;
	case 2:
		uRight = uX + 10;
		uBottom = uY + 10;
		break;
	case 3:
		uRight = uX + 20;
		uBottom = uY + 20;
		break;
	}
	uLeft = uX;
	uTop = uY;

	// 충돌 체크 확인 하기
	if (mRight >= uLeft && mBottom > uTop && mLeft <= uRight && mTop < uBottom)
	{
		return true;
	}
	else {
		return false;
	}
}

void send_crash_player(int ci) {
	g_Clients[ci].hp -= 1;
	sc_packet_crash_bullet packet;	// 변경된 내용을 클라이언트에 전송해야한다.
	packet.type = SC_PACKET_CRASH_PLAYER_BULLET;
	packet.ci = ci;	// 어떤 플레이어 인지
	packet.hp = g_Clients[ci].hp;
	for (int j = 0; j < MAX_Player; ++j) {
		if (g_Clients[j].connect == true) {
			SendPacket(g_Clients[j].sock, reinterpret_cast<unsigned char *>(&packet), sizeof(packet));
		}
	}
}

void send_crash_monster(int ci) {
	monster[ci].alive = false;

	sc_packet_crash_bullet packet;	// 변경된 내용을 클라이언트에 전송해야한다.
	packet.type = SC_PACKET_CRASH_MONSTER_BULLET;
	packet.ci = ci;	// 어떤 몬스터 인지
	packet.hp = 0;
	for (int j = 0; j < MAX_Player; ++j) {
		if (g_Clients[j].connect == true) {
			SendPacket(g_Clients[j].sock, reinterpret_cast<unsigned char *>(&packet), sizeof(packet));
		}
	}
}

bool skill_activation() {
	Location L1, L2;
	L1 = g_Clients[0].position;
	L1.x += PLAYER_SIZE / 2;
	L1.y += PLAYER_SIZE / 2;

	L2 = g_Clients[1].position;
	L2.x += PLAYER_SIZE / 2;
	L2.y += PLAYER_SIZE / 2;

	if (get_distance(L1, L2) <= 400) {
		printf("스킬 활성화됨!\n");
		return true;
	}
	else {
		printf("스킬 비활성화됨!\n");
		return false;
	}
}

int get_distance(Location L1, Location L2) {
	return sqrt(pow(L2.x, 2) - pow(L1.x, 2) + pow(L2.y, 2) - pow(L1.y, 2));
}

void send_skill_activation() {
	sc_packet_skiil_activate packet;
	packet.type = SC_PACKET_SKIIL_ACTIVATE;
	packet.ativate = g_Clients[0].skill;
	for (int i = 0; i < MAX_Player; ++i) {
		if (g_Clients[i].connect == true) {
			SendPacket(g_Clients[i].sock, reinterpret_cast<unsigned char *>(&packet), sizeof(packet));
		}
	}
	printf("변경사항 전송함!\n");
}