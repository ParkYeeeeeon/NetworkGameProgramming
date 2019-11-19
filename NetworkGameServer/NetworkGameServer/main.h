#ifndef __MAIN_H__
#define __MAIN_H__
#pragma comment(lib, "ws2_32")

#include <WinSock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <thread>
#include <Windows.h>



#define _WINSOCK_DEPRECATED_NO_WARNINGS	// 윈속 오류 방지
#define MAX_Player 2


struct CLIENT {
	int cliend_id = -1;							// 몇번째 접근한 클라이언트 인지 확인.
	bool connect = false;						// 서버와 연결을 했는지 확인.
	float x = 300, y = 300;						// 캐릭터 좌표 정보
	int hp = 100;								// 캐릭터 HP 관련 정보
	bool live = true;							// 인게임중 살아 있는지 확인
	bool Game_Play = false;						// 게임 플레이 중인지 확인
	char nickName[16];							// 캐릭터 닉네임
};

void init();														// 소켓 초기화 및 클라이언트 데이터 초기화
void err_quit( char *msg );									// 소켓 오류시 에러 출력 해주는 함수
void err_display( char *msg );								// 소켓 오류시 에러 출력 해주는 함수
int recvn( SOCKET s, char *buf, int len, int flags );		// Recv 받을때 사용 하는 함수

void Accept_Thread();											// 클라 들어오는걸 받아주는 Thread
void ProcessPacket( int ci, char *packet );				// packet 처리를 하는 부분
DWORD WINAPI Work_Thread( LPVOID arg );			// 클라에서 전송된 Packet을 읽어서 처리해준다.
void SendPacket( SOCKET sock, void *packet, int packet_size );		// 패킷을 클라에게 보낼때 하는 함수

#endif