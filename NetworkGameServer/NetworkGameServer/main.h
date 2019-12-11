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

#define _WINSOCK_DEPRECATED_NO_WARNINGS	// ���� ���� ����
#define MAX_Player 2
#define WndX 1080	
#define WndY 720
#define MAX_BUFSIZE 1024

struct CLIENT {
	SOCKET sock;
	int cliend_id = -1;							// ���° ������ Ŭ���̾�Ʈ ���� Ȯ��.
	bool connect = false;						// ������ ������ �ߴ��� Ȯ��.
	int hp;								// ĳ���� HP ���� ����
	bool live = true;							// �ΰ����� ��� �ִ��� Ȯ��
	bool Game_Play = false;						// ���� �÷��� ������ Ȯ��
	char nickName[16];							// ĳ���� �г���
	Location position;								// ĳ���� ��ǥ ����
	bool Attack = false;
	int item;
	std::list<Bullet> bullet;
	bool ready;
	int bullet_push_time = 0;		// bullet�� �ʹ� ���� �ð��ȿ� �߰� ���� �ʰ� ����� ����
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

void init();														// ���� �ʱ�ȭ �� Ŭ���̾�Ʈ ������ �ʱ�ȭ
void err_quit( char *msg );									// ���� ������ ���� ��� ���ִ� �Լ�
void err_display( char *msg );								// ���� ������ ���� ��� ���ִ� �Լ�
int recvn( SOCKET s, char *buf, int len, int flags );		// Recv ������ ��� �ϴ� �Լ�

void Accept_Thread();											// Ŭ�� �����°� �޾��ִ� Thread
void ProcessPacket( int ci, char *packet );				// packet ó���� �ϴ� �κ�
DWORD WINAPI Work_Thread(void* parameter);			// Ŭ�󿡼� ���۵� Packet�� �о ó�����ش�.
DWORD WINAPI Timer_Thread(void* parameter);					// Ÿ�̸� ������
DWORD WINAPI Calc_Thread(void* parameter);			// ���� ������

void SendPacket( SOCKET sock, void *packet, int packet_size );		// ��Ŷ�� Ŭ�󿡰� ������ �ϴ� �Լ�
void SendPacketQueue();		// Queue�� ���� ��Ŷ�� �����Ѵ�.
void DisconnectClient(int ci);				// Ŭ���̾�Ʈ ������ ���� ���� ��� ó�� �Լ�
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