#include "main.h"
#include "protocol.h"

void err_quit( char *msg ) {
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
		(LPTSTR)&lpMsgBuf, 0, NULL );
	MessageBox( NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR );
	LocalFree( lpMsgBuf );
	exit( 1 );
}

void err_display( char *msg ) {
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
		(LPTSTR)&lpMsgBuf, 0, NULL );
	printf( "[%s] %s", msg, (char *)lpMsgBuf );
	LocalFree( lpMsgBuf );
}

int recvn( SOCKET s, char *buf, int len, int flags ) {
	int received;
	char *ptr = buf;
	int left = len;

	while ( left > 0 ) {
		received = recv( s, ptr, left, flags );
		if ( received == SOCKET_ERROR )
			return SOCKET_ERROR;
		else if ( received == 0 )
			break;
		left -= received;
		ptr += received;
	}

	return (len - left);
}

void SendPacket( SOCKET sock, void *packet, int packet_size ) {
	// 고정 길이 먼저 보내주기.
	int retval = send( sock, (char *)&packet_size, sizeof( int ), 0 );
	if ( retval == SOCKET_ERROR ) {
		err_display( "send()" );
	}

	// 실제 데이터 보내주기
	char *buf = new char[packet_size];
	memcpy( buf, packet, packet_size );
	retval = send( sock, buf, packet_size, 0 );
	free(buf);
	if ( retval == SOCKET_ERROR ) {
		err_display( "send()" );
	}
}