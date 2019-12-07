#include "main.h"
#include "protocol.h"


CLIENT g_Clients[MAX_Player];

Enemy monster[MONSTER_COUNT];

SOCKET listen_sock;
HANDLE tThread, cThread;

bool playGame = true;	// �θ� �̻� ���Դ��� �Ǵ� ����

clock_t send_time;

UI ui;

int main() {
	init();
	init_monster();
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
		g_Clients[new_id].position.x = 0;
		if (new_id % 2 == 0) {
			g_Clients[new_id].position.y = 100;
		}
		else {
			g_Clients[new_id].position.y = 400;
		}
		g_Clients[new_id].bullet.reserve(PLAYER_BULLET_LIMIT);		// �Ѿ��� �ִ� 100�������� ���� �ǰ� ����
		//------------------------------------------------------------------------------------------
		// Ŭ���̾�Ʈ ������ȣ ������
		sc_packet_cino packet;
		packet.type = SC_PACKET_CINO;
		packet.no = new_id;
		SendPacket(client_sock, reinterpret_cast<unsigned char *>(&packet), sizeof(packet));
		printf("Client No : %d\n", new_id);
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
		// �̹� ���� �������� ���� ���¸� �˸���.
		for (int i = 0; i < MAX_Player; ++i) {
			// �ڽ��� ��� CINO�� ���� ���θ� �����ֹǷ� �н� �Ѵ�.
			if (i == new_id)
				continue;
			// ������ �Ǿ� ���� �ʴ� Ŭ���̾�Ʈ���Դ� ������ �ʴ´�.
			if (g_Clients[i].connect == false)
				continue;
			connect_player(i, new_id, true);
		}
		//------------------------------------------------------------------------------------------
		// ���� ���� ������ �̹� ���� �������� ���¸� �޴´�.
		for (int i = 0; i < MAX_Player; ++i) {
			// �ڽ��� ��� CINO�� ���� ���θ� �����ֹǷ� �н� �Ѵ�.
			if (i == new_id)
				continue;
			// ������ �Ǿ� ���� �ʴ� Ŭ���̾�Ʈ���Դ� ������ �ʴ´�.
			if (g_Clients[i].connect == false)
				continue;
			connect_player(new_id, i, true);
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
	int len;						// ������ ũ��


	Thread_Parameter* params = (Thread_Parameter*)parameter;

	printf("Thread Client ID : %d\n", params->ci);
	SOCKET client_sock = params->sock;
	SOCKADDR_IN clientaddr;

	// Ŭ���̾�Ʈ ���� ���
	int addrlen = sizeof(clientaddr);
	getpeername(client_sock, (SOCKADDR *)&clientaddr, &addrlen);


	while (1) {
		int retval = recvn(client_sock, (char *)&len, sizeof(int), 0); // ������ �ޱ�(���� ����)
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			DisconnectClient(params->ci);
			closesocket(client_sock);
			break;
		}

		if (len > 0) { // ���� ���̰� �������� ��츸 ���� ���̸� �޴´�.
			//printf( "Packet Size : %d\n", len );

			char *buf = new char[len];
			retval = recvn(client_sock, buf, len, 0); // ������ �ޱ�(���� ����)
			if (retval == SOCKET_ERROR) {
				err_display("recv()");
				DisconnectClient(params->ci);
				closesocket(client_sock);
				break;
			}
			//buf[retval] = '\0';
			//printf( "Packet 0��° : %d\n", buf[0] );
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
			printf("[%d] �Ѿ� ���� %d\t", ci, g_Clients[ci].bullet.size());

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
	packet.connect = value;
	SendPacket(g_Clients[me].sock, reinterpret_cast<unsigned char *>(&packet), sizeof(packet));
}

DWORD WINAPI Timer_Thread(void* parameter) {
	int StartTime, bulletTime, monsterTime, monsterSendTime;
	bool firstInit = false;
	StartTime = GetTickCount();
	bulletTime = GetTickCount();
	monsterTime = GetTickCount();
	monsterSendTime = GetTickCount();
	int progress_time = 0;
	while (1) {
		// 2���� ��� ���� ��� ����
		int new_id = -1;
		for (int i = 0; i < MAX_Player; ++i) {
			if (g_Clients[i].connect == false) {
				new_id = i;
				break;
			}
		}

		// 2���� ��� ������ ���
		if (true) {
			// 2���� ������ ��� playGame�� True�� �����Ͽ� �ٸ� �������� �ش� ������ 2���� ������ �Ǵ��Ѵ�.
			playGame = true;

			// 2�� �̻� ���� ����, ������ ó���� �����ϰ� ó��
			if (firstInit == false) {
				firstInit = true;

			}



			// ���� ��ü �ð� ���� Ÿ�̸� ó�� [1�� ���� ó���� �Ѵ�.]
			if (GetTickCount() - StartTime >= 1000) {
				//1�ʰ� ����
				//printf("%d�� ���\t", progress_time);
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

			// �Ѿ� ������ Ÿ�̸� ó�� [0.05�� ���� ó���� �Ѵ�.]
			if (GetTickCount() - bulletTime >= 50) {
				bulletTime = GetTickCount();

				// �Ѿ� �߻� �� �̵�
				for (int i = 0; i < MAX_Player; ++i) {
					// Ŭ���̾�Ʈ�� ������ ���� ��쿡�� ó��
					if (g_Clients[i].connect == true) {
						//add_player_bullet(g_Clients);	// �̹� Attack Packet���� ������ ���ش�.

						add_bullet_position(i);
					}
				}
			}

			// ���� ������ ó�� [0.1�� ���� ó���� �Ѵ�.]
			if (GetTickCount() - monsterTime >= 100) {
				monsterTime = GetTickCount();
				move_monster_location();
			}

			// ���� ������ ����ȭ ó�� [5�� ���� ó���� �Ѵ�.]
			if (GetTickCount() - monsterSendTime >= 5000) {
				monsterSendTime = GetTickCount();
				for (int i = 0; i < MAX_Player; ++i) {
					if (g_Clients[i].connect == true) {
						send_monster_location(i);
					}
				}
			}


		}
		else {
			//2���� ������ �ʾ��� ��� false ó�� �Ѵ�.
			playGame = false;
		}


	}
	return 0;
}

DWORD WINAPI Calc_Thread(void* parameter) {
	//while (1) {
	//	// 2���� ��� ������ ���
	//	if (playGame == true) {
	//		// �Ѿ� �߻� �� �̵�
	//		for (int i = 0; i < MAX_Player; ++i) {
	//			// Ŭ���̾�Ʈ�� ������ ���� ��쿡�� ó��
	//			if (g_Clients[i].connect == true) {
	//				//add_player_bullet(g_Clients);	// �̹� Attack Packet���� ������ ���ش�.
	//				add_bullet_position(i);
	//			}
	//		}
	//	}
	//}
	return 0;
}

void add_bullet_position(int ci) {
	if (g_Clients[ci].bullet.size() > 0) {
		// Vector �� ��ȸ�ϸ�, �Ѿ� ��ġ�� �̵� �����ش�.
		for (std::vector<Bullet>::iterator it = g_Clients[ci].bullet.begin(); it != g_Clients[ci].bullet.end(); ) {
			// �Ѿ��� ������ ��� ��� ���� ó���� �Ѵ�.
			if (it->position.x >= WndX) {
				it = g_Clients[ci].bullet.erase(it);
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
	}
}

void move_monster_location() {
	for (int i = 0; i < MAX_Player; ++i) {
		// Ŭ���̾�Ʈ�� ������ �ȵǾ� ���� ��� �������� �ʴ´�.
		if (g_Clients[i].connect == false)
			continue;


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