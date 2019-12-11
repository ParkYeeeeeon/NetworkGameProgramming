#include "main.h"
#include "protocol.h"


CLIENT g_Clients[MAX_Player];

Enemy monster[MONSTER_COUNT];
Enemy boss;
std::list<Bullet> bullet;
std::list<Bullet> bossBullet;
SOCKET listen_sock;
HANDLE tThread, cThread;

bool playGame = false;	// �θ� �̻� ���Դ��� �Ǵ� ����

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
		int new_id = -1;	// �� ��° Ŭ���̾�Ʈ ���� Ȯ�� �ϱ� ���� ����
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
			printf("������ �ο� �̻����� �����Ͽ� �����Ͽ����ϴ�..!\n");
			closesocket(client_sock);
			continue;
		}
		//------------------------------------------------------------------------------------------
		// ĳ���� �ʱ�ȭ.
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
		// Ŭ���̾�Ʈ ������ȣ ������
		sc_packet_cino packet;
		packet.type = SC_PACKET_CINO;
		packet.no = new_id;
		SendPacket(client_sock, reinterpret_cast<unsigned char *>(&packet), sizeof(packet));
		printf("Client No : %d\n", new_id);

		//------------------------------------------------------------------------------------------
		// �̹� ���� �������� ���� ���¸� �˸���.
		for (int i = 0; i < MAX_Player; ++i) {
			// ������ �Ǿ� ���� �ʴ� Ŭ���̾�Ʈ���Դ� ������ �ʴ´�.
			if (g_Clients[i].connect == false)
				continue;
			connect_player(i, new_id, true);
		}
		//------------------------------------------------------------------------------------------
		// ���� ���� ������ �̹� ���� �������� ���¸� �޴´�.
		for (int i = 0; i < MAX_Player; ++i) {
			// ������ �Ǿ� ���� �ʴ� Ŭ���̾�Ʈ���Դ� ������ �ʴ´�.
			if (g_Clients[i].connect == false)
				continue;
			connect_player(new_id, i, true);
		}

		//------------------------------------------------------------------------------------------
		// Ŭ���̾�Ʈ ���� ���� ������
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
		// Work_Thread ����
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
	// ���� ����
	WSACleanup();
}

void DisconnectClient(int ci) {
	printf("Client ID : %d Disconnect\n", ci);
	g_Clients[ci].connect = false;

	// ��밡 ������ ��� �����ٰ� ��� �������� ������ ���ش�.
	for (int i = 0; i < MAX_Player; ++i) {
		// �ڽ��� ��� CINO�� ���� ���θ� �����ֹǷ� �н� �Ѵ�.
		if (i == ci)
			continue;
		// ������ �Ǿ� ���� �ʴ� Ŭ���̾�Ʈ���Դ� ������ �ʴ´�.
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

	// Ŭ���̾�Ʈ ���� ���
	int addrlen = sizeof(clientaddr);
	getpeername(client_sock, (SOCKADDR *)&clientaddr, &addrlen);


	while (1) {
		//------------------------------------------------------------------------------------------
		int len;						// ������ ũ��
		int retval = recvn(client_sock, (char *)&len, sizeof(int), 0); // ������ �ޱ�(���� ����)
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			DisconnectClient(params->ci);
			closesocket(client_sock);
			break;
		}

		char buf[MAX_BUFSIZE]{ 0 };
		retval = recvn(client_sock, buf, len, 0); // ������ �ޱ�(���� ����)
		//retval = recv(client_sock, buf, len, 0);
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			DisconnectClient(params->ci);
			closesocket(client_sock);
			break;
		}
		buf[retval] = '\0';

		if ((int)buf[0] >= LIMIT_PACKET_CLIENT_NO || (int)buf[0] <= 0) {
			printf("[%d] ���������� ���� ��Ŷ�� �߼� �Ǿ����ϴ�..!\n", params->ci);
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
		// Ŭ���̾�Ʈ���� Ready ��ư�� ������ ��� �غ� �Ϸ�� ���·� �ٲ۴�.
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

		send_location_packet(ci, ci);	// ���� ������� �ڽ��� ��ġ�� �����ش�.
		for (int i = 0; i < MAX_Player; ++i) {
			// �ڽ��� ���̵� ������ �ٸ� ����鿡�� ���� ����� ��ġ�� �����ش�.
			if (i == ci)
				continue;
			send_location_packet(i, ci);
		}
	}
	break;


	case CS_PACKET_ATTACK:
	{
		// ���� ��Ŷ�� �޴���, 2���̻� ��� ���� ��쿡�� ó���� �Ѵ�.
		// 0.2�ʿ� �ѹ��� �Ѿ��� �����ϰ� ������.
		if (playGame == true && GetTickCount() - g_Clients[ci].bullet_push_time >= 200) {
			// Vector�� �Ѿ� ������ PLAYER_BULLET_LIMIT ũ�ų� ���� ��쿡�� ���̻� ���� ���� �ʴ´�.
			if (g_Clients[ci].bullet.size() >= PLAYER_BULLET_LIMIT)
				break;

			cs_packet_attack *attack_packet = reinterpret_cast<cs_packet_attack *>(packet);
			//printf("[%d] �Ѿ� ���� %d\t", ci, g_Clients[ci].bullet.size());

			// �Ѿ� ������ ���� �÷��̾� ��ġ ���
			Location temp_location;
			temp_location.x = g_Clients[ci].position.x + 55;
			temp_location.y = g_Clients[ci].position.y + 20;

			// Vector�� ���� �Ѿ� ����
			Bullet bullet;
			bullet.position = temp_location;		// �Ѿ��� ��ġ�� ���� �÷��̾��� ��ġ�� �־��ش�.
			bullet.type = 5;	// add_player_bullet ���� type 5��� ���� �־ 5��� ����
			bullet.dir = 0; // �÷��̾� ��ü�� �Ѿ� ������ ���ڷ� �����ٸ� ������ �ִ´�.
			bullet.bullet_type = 0; // �Ѿ� Ÿ�Ե� ���� ��� ������ �ִ´�.
			bullet.draw = true;		// ȭ�鿡 �׷���� �ϹǷ� true
			g_Clients[ci].bullet.emplace_back(bullet);	// temp�� ���� bullet�� vector�� �־��ش�.
			g_Clients[ci].bullet_push_time = GetTickCount();	// ���� �ð��� �־��༭ ������ 1�� �ڿ� �Ѿ��� ���� �� �ְ� ������ �Ѵ�.

			// �Ѿ� �߻� ���� Ŭ�󿡰� �Ѿ� �߻� �ߴٰ� ���� ����
			for (int i = 0; i < MAX_Player; ++i) {
				// 2���� ���� ���� ���� �Ѿ� �߻簡 �����ϹǷ�, 2���� ������ ���� ���� ó���� �Ѵ�.
				if (playGame == false)
					continue;
				// ������ �ִ� Ŭ���̾�Ʈ �鿡�� ����
				if (g_Clients[i].connect == true) {
					sc_packet_bullet packet;
					packet.type = SC_PACKET_BULLET;
					packet.no = ci;		// �Ѿ� �߻��ڰ� ��������
					packet.bullet = bullet;	// �ش� �Ѿ˿� ���� ���� ����
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
	// �÷��̾ ������ ���� ��쿡�� ��Ŷ�� ������.
	if (g_Clients[me].connect == true) {
		sc_packet_location packet;
		packet.type = SC_PACKET_DIR;
		if (me != you) {
			// ��� ��ġ�� ������. �ڽ����� ������.
			packet.ci = you;
			packet.x = g_Clients[you].position.x;
			packet.y = g_Clients[you].position.y;
			SendPacket(g_Clients[me].sock, reinterpret_cast<unsigned char *>(&packet), sizeof(packet));
		}
		else {
			// �ڱ� �ڽ��� ��ġ�� ������.
			packet.ci = me;
			packet.x = g_Clients[me].position.x;
			packet.y = g_Clients[me].position.y;
			SendPacket(g_Clients[me].sock, reinterpret_cast<unsigned char *>(&packet), sizeof(packet));
		}
	}
}

// me = �޴� �÷��̾�, you�� ���� �÷��̾�, value�� ��
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

		// 2���� ���� ���¿��� ���� ��� �Ϸ� ���� ��� �����Ѵ�.
		int new_id = 0;
		for (int i = 0; i < MAX_Player; ++i) {
			if (g_Clients[i].connect == true && g_Clients[i].ready == true) {
				new_id++;
			}
		}

		// 2���� ��� ���� �Ͽ��� ���
		if (new_id == MAX_Player) {
			// 2���� ������ ��� playGame�� True�� �����Ͽ� �ٸ� �������� �ش� ������ 2���� ������ �Ǵ��Ѵ�.
			playGame = true;

			// ó������ �Ѵ� ���� �Ͽ��� ���
			if (initPlayerReady == false) {
				// ��� ���ӵ� �÷��̾�� ready ������ �˷��ش�.
				for (int i = 0; i < MAX_Player; ++i) {
					// ������ �Ǿ� ���� �ʴ� Ŭ���̾�Ʈ���Դ� ������ �ʴ´�.
					if (g_Clients[i].connect == false)
						continue;
					for (int j = 0; j < MAX_Player; ++j) {
						connect_player(i, j, true);
					}
				}
				initPlayerReady = true;
			}


			// ���� ��ü �ð� ���� Ÿ�̸� ó�� [1�� ���� ó���� �Ѵ�.]
			if (GetTickCount() - StartTime >= 1000) {
				//1�ʰ� ����
				//printf("%d�� ���\t", progress_time);
				StartTime = GetTickCount();
				progress_time += 1;

				// ���� �⿬�ð� üũ
				if (progress_time >= BOSS_COUNT && showWarning == false) {
					// �⿬�ð��� �Ǹ� �ð��� 0���� �ʱ�ȭ �ϰ�, Warning�� ���� �ش�.
					progress_time = 0;
					showWarning = true;
					packet_type++;
				}
				else if (progress_time >= SHOW_WARNING_COUNT && showWarning == true && showBoss == false) {
					// Warning �ð��� ���� ��� ���� ������ ������ ó���� �Ѵ�.
					showBoss = true;
					packet_type++;
				}
				for (int i = 0; i < MAX_Player; ++i) {
					// ���� ���� ���� ��� �ѱ��.
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

			// �Ѿ� ������ Ÿ�̸� ó�� [0.05�� ���� ó���� �Ѵ�.]
			if (GetTickCount() - bulletTime >= 50) {
				bulletTime = GetTickCount();

				// �Ѿ� �߻� �� �̵�
				for (int i = 0; i < MAX_Player; ++i) {
					// Ŭ���̾�Ʈ�� ������ ���� ��쿡�� ó��
					if (g_Clients[i].connect == true) {
						add_bullet_position(i);
					}
				}
			}

			// ����, ���� ������ ó�� [0.1�� ���� ó���� �Ѵ�.]
			if (GetTickCount() - monsterTime >= 100) {
				monsterTime = GetTickCount();
				move_monster_location();
				// ���� ������ ó��
				if (showBoss == true) {
					move_boss_location();
					if (boss.hp <= 0) {
						showBoss = false;
						showWarning = false;
						progress_time = 0;
						packet_type = 0;
						// ��Ŷ Ÿ�Ը� �����ִ� ó�� [���� ���̶�� ���� Ÿ�Ը� �����ش�.]
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

			// ���� ������ ����ȭ ó�� [5�� ���� ó���� �Ѵ�.]
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

			// ���� �Ѿ�, �츲 ó�� [2�� ���� ó���� �Ѵ�.]
			if (GetTickCount() - monsterBullet >= 2000) {
				monsterBullet = GetTickCount();
				add_enemy_bullet();
				revival_enemy();	// 2�ʸ��� ���� �ٽ� ����
			}

			// ����  �Ѿ� [0.5�� ���� ó���� �Ѵ�.]
			if (GetTickCount() - bossBulletTime >= 500) {
				bossBulletTime = GetTickCount();
				if (showBoss == true) {
					add_boss_bullet(progress_time);
				}
			}

			// ���� �Ѿ� ������ ó�� [0.001�� ���� ó���� �Ѵ�.]
			if (GetTickCount() - monsterBulletMove >= 1) {
				monsterBulletMove = GetTickCount();
				move_enemybullet();
			}

			// �� -> �÷��̾� �浹üũ [ 0.01�ʸ��� ó���� �Ѵ�. ]
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
								// �浹 ó��
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
									// �浹 ó��
									b->draw = false;
									send_crash_player(i);
								}
								++b;
							}
						}
					}
				}
			}

			// �÷��̾� -> �� �浹üũ [ 0.01�ʸ��� ó���� �Ѵ�. ]
			if (GetTickCount() - crashcheck_monster_time >= 10) {
				crashcheck_monster_time = GetTickCount();

				for (int i = 0; i < MAX_Player; ++i) {
					if (g_Clients[i].connect == true) {
						for (std::list<Bullet>::iterator b = g_Clients[i].bullet.begin(); b != g_Clients[i].bullet.end();)
						{
							// �Ѿ��� �ȱ׷����� ��� �ѱ��. [�浹üũ ���ʿ�]
							if (b->draw == false) {
								++b;
								continue;
							}
							for (int j = 0; j < MONSTER_COUNT; ++j) {
								if (monster[j].alive == false)
									continue;

								if (crash_check(monster[j].position.x, monster[j].position.y, b->position.x, b->position.y, 3, false) == true) {
									// �浹 ó��
									b->draw = false;
									monster[j].alive = false;
									send_crash_monster(i);
								}
							}

							if (showBoss == true) {
								if (crash_check(boss.position.x, boss.position.y, b->position.x, b->position.y, 3, true) == true) {
									// �浹 ó��
									b->draw = false;
									boss.hp -= 1;
								}
							}

							++b;
						}
					}
				}

			}


			// �÷��̾� ��ų üũ [ 0.01�ʸ��� üũ ]
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
			//2���� ������ �ʾ��� ��� false ó�� �Ѵ�.
			playGame = false;
			initPlayerReady = false;
		}


	}
	return 0;
}

DWORD WINAPI Calc_Thread(void* parameter) {
	//while (1) {
	//	// Packet ������ ó���� �����Ѵ�. [0.001�� ���� ó���� �Ѵ�.]
	//	// SendPacket���� �ٷ� ó���� �� ��� �߰��� �����Ͱ� ���̴� ������ ��Ÿ����.
	//	// �̸� �ذ� �ϱ� ���Ͽ� Queue�� �����͸� ���� ��
	//	// ���� �ð����� Queue���� ���� �ϴ� ������� ����
	//	SendPacketQueue();
	//}
	return 0;
}

void add_bullet_position(int ci) {
	if (g_Clients[ci].bullet.size() > 0) {
		// Vector �� ��ȸ�ϸ�, �Ѿ� ��ġ�� �̵� �����ش�.
		for (std::list<Bullet>::iterator it = g_Clients[ci].bullet.begin(); it != g_Clients[ci].bullet.end(); ) {
			// �Ѿ��� ������ ��� ��� ���� ó���� �Ѵ�.
			if (it->position.x >= WndX || it->draw == false) {
				it = g_Clients[ci].bullet.erase(it);
				continue;
			}
			else {
				// �Ѿ� ���� �̵�
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
	for (int i = 0; i < MONSTER_COUNT; i++) // ���� ��ġ ���� ����
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
		// ���� ��ġ�� ���� �̵� ��Ų��.
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
			// ������ ����� �׾��ٰ� ó���� �Ѵ�.
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
		// ���Ͱ� ��� ���� ��쿡�� ó�� �Ѵ�.
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
				tmp_bullet.dir = j;	// ���� // ���� ���� // ���� �Ʒ�
				bullet.push_back(tmp_bullet);
			}
		}
		break;

		}
	}

	// �Ѿ��� �߻� �ߴٴ� ����� �����Ѵ�.
	// ����, Ŭ�� ���� ����� ó���Ѵ�.
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
			tmp_bullet.dir = j;	// ���� // ���� ���� // ���� �Ʒ�
			bossBullet.push_back(tmp_bullet);
		}
	}
	break;

	}

	// �Ѿ��� �߻� �ߴٴ� ����� �����Ѵ�.
	// ����, Ŭ�� ���� ����� ó���Ѵ�.
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
		// �Ѿ��� ���� ������ ������ ��� ����
		if ((i->position.x <= 0 || i->position.y <= 0 || i->position.y >= 720) || i->draw == false) {
			// ������ ����� ������ ���ش�.
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
	// �� �Ѿ� �׸���
		// ���� �Ʒ�
	switch (i->dir) {
	case 0:
		// ����
		i->position.x -= 3;
		break;
	case 1:
		// ���� ����
		i->position.x -= 3;
		i->position.y += 3;
		break;
	case 2:
		// ���� �Ʒ�
		i->position.x -= 3;
		i->position.y -= 3;
		break;
	}
}

void revival_enemy() {
	// ���Ͱ� �׾��� ��� ������ ��ġ�� �ٽ� ���� ��Ų��.
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
	// �÷��̾� �簢�� �ڽ� �����
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

	// �Ѿ� �簢�� �ڽ� �����
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

	// �浹 üũ Ȯ�� �ϱ�
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
	sc_packet_crash_bullet packet;	// ����� ������ Ŭ���̾�Ʈ�� �����ؾ��Ѵ�.
	packet.type = SC_PACKET_CRASH_PLAYER_BULLET;
	packet.ci = ci;	// � �÷��̾� ����
	packet.hp = g_Clients[ci].hp;
	for (int j = 0; j < MAX_Player; ++j) {
		if (g_Clients[j].connect == true) {
			SendPacket(g_Clients[j].sock, reinterpret_cast<unsigned char *>(&packet), sizeof(packet));
		}
	}
}

void send_crash_monster(int ci) {
	monster[ci].alive = false;

	sc_packet_crash_bullet packet;	// ����� ������ Ŭ���̾�Ʈ�� �����ؾ��Ѵ�.
	packet.type = SC_PACKET_CRASH_MONSTER_BULLET;
	packet.ci = ci;	// � ���� ����
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
		printf("��ų Ȱ��ȭ��!\n");
		return true;
	}
	else {
		printf("��ų ��Ȱ��ȭ��!\n");
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
	printf("������� ������!\n");
}