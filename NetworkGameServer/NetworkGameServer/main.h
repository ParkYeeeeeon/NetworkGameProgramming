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

#include "Object.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS	// ���� ���� ����
#define MAX_Player 2
#define WndX 1080	
#define WndY 720

struct CLIENT {
	SOCKET sock;
	int cliend_id = -1;							// ���° ������ Ŭ���̾�Ʈ ���� Ȯ��.
	bool connect = false;						// ������ ������ �ߴ��� Ȯ��.
	//float x = 300, y = 300;						// ĳ���� ��ǥ ����
	int hp = 100;								// ĳ���� HP ���� ����
	bool live = true;							// �ΰ����� ��� �ִ��� Ȯ��
	bool Game_Play = false;						// ���� �÷��� ������ Ȯ��
	char nickName[16];							// ĳ���� �г���
	Location position;
	bool Attack = false;
	std::vector<Bullet> bullet;
	// Player PLAYER;
};

struct Thread_Parameter {
	SOCKET sock;
	int ci;
};


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
void DisconnectClient(int ci);				// Ŭ���̾�Ʈ ������ ���� ���� ��� ó�� �Լ�
void player_move_process(int ci);
void send_location_packet(int me, int you);
void connect_player(int me, int you, bool value);

void Send_Player_Bullet(int ci);
void add_player_bullet(CLIENT player[2]);
void add_bullet_position(CLIENT player[2]);

#endif