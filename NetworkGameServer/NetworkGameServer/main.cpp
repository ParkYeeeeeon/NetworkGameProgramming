#include "protocol.h"
#include "main.h"

CLIENT g_Clients[MAX_Player];
SOCKET listen_sock;

int main() {
	init();

	std::thread accept_thread{ Accept_Thread };
	//std::thread work_thread{ Work_Thread };
	//work_thread.join();
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
		int new_id = -1;	// 몇 번째 클라이언트 인지 확인 하기 위한 변수
		for ( int i = 0; i < MAX_Player; ++i ) {
			if ( g_Clients[i].connect == false ) {
				new_id = i;
				break;
			}
		}
		//------------------------------------------------------------------------------------------
		if ( -1 == new_id ) {
			printf( "설정된 인원 이상으로 접속하여 차단하였습니다..!\n" );
			closesocket( client_sock );
			continue;
		}
		//------------------------------------------------------------------------------------------
		// 캐릭터 초기화.
		g_Clients[new_id].cliend_id = new_id;
		g_Clients[new_id].connect = true;

		//------------------------------------------------------------------------------------------
		// Work_Thread 시작
		hThread = CreateThread( NULL, 0, Work_Thread, (LPVOID)client_sock, 0, NULL );
		if ( hThread == NULL ) {
			closesocket( client_sock );
		}
		else {
			CloseHandle( hThread );
		}
		//------------------------------------------------------------------------------------------
	}
	closesocket( listen_sock );
	// 윈속 종료
	WSACleanup();
}

DWORD WINAPI Test_Thread( LPVOID arg ) {
	int len;						// 데이터 크기
	SOCKET client_sock = (SOCKET)arg;
	SOCKADDR_IN clientaddr;

	// 클라이언트 정보 얻기
	int addrlen = sizeof( clientaddr );
	getpeername( client_sock, (SOCKADDR *)&clientaddr, &addrlen );

	while ( 1 ) {
		//scanf( "%d", &test_scan );
		int test_scan = 1;
		switch (test_scan) {
		}
	}
	return 0;
}


DWORD WINAPI Work_Thread( LPVOID arg ) {
	int len;						// 데이터 크기
	//char buf[MAX_Bufsize];	// 가변 길이 데이터 받아오는 버퍼

	SOCKET client_sock = (SOCKET)arg;
	SOCKADDR_IN clientaddr;

	// 클라이언트 정보 얻기
	int addrlen = sizeof( clientaddr );
	getpeername( client_sock, (SOCKADDR *)&clientaddr, &addrlen );

	HANDLE hThread = CreateThread( NULL, 0, Test_Thread, (LPVOID)client_sock, 0, NULL );

	while ( 1 ) {
		int retval = recvn( client_sock, (char *)&len, sizeof( int ), 0 ); // 데이터 받기(고정 길이)
		if ( retval == SOCKET_ERROR ) {
			err_display( "recv()" );
			break;
		}

		if ( len >= 0 ) { // 고정 길이가 정상적일 경우만 가변 길이를 받는다.
			printf( "Packet Size : %d\n", len );

			char *buf = new char[len];
			retval = recvn( client_sock, buf, len, 0 ); // 데이터 받기(가변 길이)
			if ( retval == SOCKET_ERROR ) {
				err_display( "recv()" );
				break;
			}
			//buf[retval] = '\0';
			printf( "Packet 0번째 : %d\n", buf[0] );
			ProcessPacket( 0, buf );

		}


	}
	CloseHandle( hThread );
	return 0;
}

void ProcessPacket( int ci, char *packet ) {
	switch ( packet[0] ) {
	

	}
}