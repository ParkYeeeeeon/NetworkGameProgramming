#ifndef __MAIN_H__
#define __MAIN_H__

#pragma comment(lib, "ws2_32")

#include <WinSock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <thread>
#include <Windows.h>
#include <time.h>
#include <ctime>
#include <deque>
#include <list>

#include "Object.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS	// 윈속 오류 방지
#define MAX_Player 2
#define WndX 1080	
#define WndY 720
#define MAX_BUFSIZE 1024

struct CLIENT {
	SOCKET sock;
	int cliend_id = -1;							// 몇번째 접근한 클라이언트 인지 확인.
	bool connect = false;						// 서버와 연결을 했는지 확인.
	int hp;								// 캐릭터 HP 관련 정보
	bool live = true;							// 인게임중 살아 있는지 확인
	bool Game_Play = false;						// 게임 플레이 중인지 확인
	char nickName[16];							// 캐릭터 닉네임
	Location position;								// 캐릭터 좌표 정보
	bool Attack = false;
	int item;
	std::list<Bullet> bullet;
	bool ready;
	int bullet_push_time = 0;		// bullet을 너무 빠른 시간안에 추가 하지 않게 만들기 위해
};

struct Thread_Parameter {
	SOCKET sock;
	int ci;
};

struct Packet_Queue {
	SOCKET sock;
	//void *packet;
	char buf[MAX_BUFSIZE]{ 0 };
	int packet_size;
};

extern std::deque<Packet_Queue> packet_queue;

void init();														// 소켓 초기화 및 클라이언트 데이터 초기화
void err_quit( char *msg );									// 소켓 오류시 에러 출력 해주는 함수
void err_display( char *msg );								// 소켓 오류시 에러 출력 해주는 함수
int recvn( SOCKET s, char *buf, int len, int flags );		// Recv 받을때 사용 하는 함수

void Accept_Thread();											// 클라 들어오는걸 받아주는 Thread
void ProcessPacket( int ci, char *packet );				// packet 처리를 하는 부분
DWORD WINAPI Work_Thread(void* parameter);			// 클라에서 전송된 Packet을 읽어서 처리해준다.
DWORD WINAPI Timer_Thread(void* parameter);					// 타이머 스레드
DWORD WINAPI Calc_Thread(void* parameter);			// 연산 스레드

void SendPacket( SOCKET sock, void *packet, int packet_size );		// 패킷을 클라에게 보낼때 하는 함수
void SendPacketQueue();		// Queue에 넣은 패킷을 전송한다.
void DisconnectClient(int ci);				// 클라이언트 연결이 끊어 졌을 경우 처리 함수
void send_location_packet(int me, int you);
void connect_player(int me, int you, bool value);

void add_player_bullet(CLIENT player[2]);
void add_bullet_position(int ci);

void init_monster();
void init_boss();
void move_monster_location();
void move_boss_location();
void send_monster_location(int ci);
void send_boss_location(int ci);
void add_enemy_bullet();
void add_boss_bullet(int time);
void move_monster_location();
void move_enemybullet();
void change_enemy_bullet(std::list<Bullet>::iterator i);
void revival_enemy();
bool crash_check(int myX, int myY, int uX, int uY, int uType, bool isBoss);
void send_crash_player(int ci);
void send_crash_monster(int ci);
#endif