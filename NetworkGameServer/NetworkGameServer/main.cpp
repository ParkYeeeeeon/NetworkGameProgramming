#include "protocol.h"
#include "main.h"

CLIENT g_Clients[MAX_Player];
SOCKET listen_sock;

int main() {
	init();

	std::thread accept_thread{ Accept_Thread };
	accept_thread.join();
}

void init() {	
	WSADATA wsa;
	WSAStartup( MAKEWORD( 2, 2 ), &wsa );
	listen_sock = socket( AF_INET, SOCK_STREAM, 0 );

	if ( listen_sock == INVALID_SOCKET ) err_quit( "socket()" );

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory( &serveraddr, sizeof( serveraddr ) );
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl( INADDR_ANY );
	serveraddr.sin_port = htons( SERVERPORT );
	bool flag = TRUE;
	setsockopt( listen_sock, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof( flag ) );

	int retval = bind( listen_sock, (SOCKADDR *)&serveraddr, sizeof( serveraddr ) );
	if ( retval == SOCKET_ERROR ) err_quit( "bind()" );

	// listen()
	retval = listen( listen_sock, SOMAXCONN );
	if ( retval == SOCKET_ERROR ) err_quit( "listen()" );

	for ( int i = 0; i < MAX_Player; ++i )
		g_Clients[i].connect = false;

	printf("Server init Complete..!\n");
}

void Accept_Thread() {
	SOCKADDR_IN clientaddr;
	HANDLE hThread;
	while ( 1 ) {
		// accept()
		int addrlen = sizeof( clientaddr );
		SOCKET client_sock = accept( listen_sock, (SOCKADDR *)&clientaddr, &addrlen );
		if ( client_sock == INVALID_SOCKET ) {
			err_display( "accept()" );
			break;
		}
		printf( "Client IP : %s, Port : %d\n", inet_ntoa( clientaddr.sin_addr ), ntohs( clientaddr.sin_port ) );

		//------------------------------------------------------------------------------------------
		int new_id = -1;	// �� ��° Ŭ���̾�Ʈ ���� Ȯ�� �ϱ� ���� ����
		for ( int i = 0; i < MAX_Player; ++i ) {
			if ( g_Clients[i].connect == false ) {
				new_id = i;
				break;
			}
		}
		//------------------------------------------------------------------------------------------
		if ( -1 == new_id ) {
			printf( "������ �ο� �̻����� �����Ͽ� �����Ͽ����ϴ�..!\n" );
			closesocket( client_sock );
			continue;
		}
		//------------------------------------------------------------------------------------------
		// ĳ���� �ʱ�ȭ.
		g_Clients[new_id].cliend_id = new_id;
		g_Clients[new_id].connect = true;
		//------------------------------------------------------------------------------------------
		// Ŭ���̾�Ʈ ������ȣ ������
		sc_packet_cino packet;
		packet.type = SC_PACKET_CINO;
		packet.no = new_id;
		SendPacket(client_sock, reinterpret_cast<unsigned char *>(&packet), sizeof(packet));
		printf("Client No : %d\n", new_id);
		//------------------------------------------------------------------------------------------
		// Work_Thread ����
		Thread_Parameter* params = new Thread_Parameter;
		params->sock = client_sock;
		params->ci = new_id;

		hThread = CreateThread( NULL, 0, Work_Thread, params, 0, NULL );
		if ( hThread == NULL ) {
			closesocket( client_sock );
		}
		else {
			CloseHandle( hThread );
		}
		//------------------------------------------------------------------------------------------
	}
	closesocket( listen_sock );
	// ���� ����
	WSACleanup();
}

void DisconnectClient(int ci) {
	printf("Client ID : %d Disconnect\n", ci);
	g_Clients[ci].connect = false;
}

DWORD WINAPI Work_Thread(void* parameter) {
	int len;						// ������ ũ��

	Thread_Parameter* params = (Thread_Parameter*)parameter;

	printf("Thread Client ID : %d\n", params->ci);
	SOCKET client_sock = params->sock;
	SOCKADDR_IN clientaddr;

	// Ŭ���̾�Ʈ ���� ���
	int addrlen = sizeof( clientaddr );
	getpeername( client_sock, (SOCKADDR *)&clientaddr, &addrlen );


	while ( 1 ) {
		int retval = recvn( client_sock, (char *)&len, sizeof( int ), 0 ); // ������ �ޱ�(���� ����)
		if ( retval == SOCKET_ERROR ) {
			err_display( "recv()" );
			DisconnectClient(params->ci);
			closesocket(client_sock);
			break;
		}

		if ( len >= 0 ) { // ���� ���̰� �������� ��츸 ���� ���̸� �޴´�.
			//printf( "Packet Size : %d\n", len );

			char *buf = new char[len];
			retval = recvn( client_sock, buf, len, 0 ); // ������ �ޱ�(���� ����)
			if ( retval == SOCKET_ERROR ) {
				err_display( "recv()" );
				DisconnectClient(params->ci);
				closesocket(client_sock);
				break;
			}
			//buf[retval] = '\0';
			//printf( "Packet 0��° : %d\n", buf[0] );
			ProcessPacket( 0, buf );

		}


	}
	return 0;
}

void ProcessPacket( int ci, char *packet ) {
	switch ( packet[0] ) {
	case CS_PACKET_DIR:
		cs_packet_dir *my_packet = reinterpret_cast<cs_packet_dir *>(packet);
		printf("[%d] : %d\n", ci, my_packet->dir);
		switch (my_packet->dir)
		{
		case VK_UP_UP:
			printf("[%d] : %d\n", ci, VK_UP_UP);
			break;
		case VK_UP_DOWN:
			printf("[%d] : %d\n", ci, VK_UP_DOWN);
			break;
		case VK_UP_LEFT:
			printf("[%d] : %d\n", ci, VK_UP_LEFT);
			break;
		case VK_UP_RIGHT:
			printf("[%d] : %d\n", ci, VK_UP_RIGHT);
			break;
		case VK_DOWN_UP:
			printf("[%d] : %d\n", ci, VK_DOWN_UP);
			break;
		case VK_DOWN_DOWN:
			printf("[%d] : %d\n", ci, VK_DOWN_DOWN);
			break;
		case VK_DOWN_LEFT:
			printf("[%d] : %d\n", ci, VK_DOWN_LEFT);
			break;
		case VK_DOWN_RIGHT:
			printf("[%d] : %d\n", ci, VK_DOWN_RIGHT);
			break;
		}
		break;

	}
}