#include "main.h"
#include "protocol.h"


CLIENT g_Clients[MAX_Player];
SOCKET listen_sock;
HANDLE tThread, cThread;

clock_t send_time;

UI ui;
Bullet Bullet_Send_player[2][200];

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
	cs_packet_dir *my_packet = reinterpret_cast<cs_packet_dir *>(packet);
	switch (packet[0]) {
	case CS_PACKET_DIR:

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
		break;


	case CS_PACKET_ATTACK:
		cs_packet_attack *attack_packet = reinterpret_cast<cs_packet_attack *>(packet);
		g_Clients[ci].Attack = attack_packet->attack;
		break;

	}

	// 2���� ��� ���� ��� ����
	int new_id = -1;
	for (int i = 0; i < MAX_Player; ++i) {
		if (g_Clients[i].connect == false) {
			new_id = i;
			break;
		}
	}

	// 2���� ��� ������ ���
	if (-1 == new_id) {
		Send_Player_Bullet(ci);
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
	int StartTime;
	StartTime = GetTickCount();
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
		if (-1 == new_id) {
			if (GetTickCount() - StartTime >= 1000) {
				//1�ʰ� ����
				printf("%d�� ���\t", progress_time);
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

DWORD WINAPI Calc_Thread(void* parameter) {
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
		if (-1 == new_id) {
			// �Ѿ� �߻� �� �̵�
			add_player_bullet(g_Clients);
			add_bullet_position(g_Clients);

		}

	}
	return 0;
}

void Send_Player_Bullet(int ci) {

	for (int i = 0; i < 2; ++i) {
		for (int j = 0; j < 200; ++j) {
			Bullet_Send_player[i][j].draw = false;
		}
	}

	for (int i = 0; i < MAX_Player; ++i) {
		for (int j = 0; j < g_Clients[i].bullet.size(); ++j) {
			Bullet_Send_player[i][j] = g_Clients[i].bullet[j];
			Bullet_Send_player[i][j].draw = true;
		}
		sc_packet_bullet packet;
		packet.type = SC_PACKET_BULLET;
		SendPacket(g_Clients[i].sock, reinterpret_cast<unsigned char *>(&packet), sizeof(packet));
	}

}


void add_bullet_position(CLIENT player[2]) {
	for (int i = 0; i < 2; ++i) {
		if (player[i].bullet.size() > 0)
			for (std::vector<Bullet>::iterator j = player[i].bullet.begin(); j < player[i].bullet.end(); ++j) {
				j->position.x += 10;
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