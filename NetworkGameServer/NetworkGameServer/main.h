#ifndef __MAIN_H__
#define __MAIN_H__
#pragma comment(lib, "ws2_32")

#include <WinSock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <thread>
#include <Windows.h>



#define _WINSOCK_DEPRECATED_NO_WARNINGS	// ���� ���� ����
#define MAX_Player 2


struct CLIENT {
	int cliend_id = -1;							// ���° ������ Ŭ���̾�Ʈ ���� Ȯ��.
	bool connect = false;						// ������ ������ �ߴ��� Ȯ��.
	float x = 300, y = 300;						// ĳ���� ��ǥ ����
	int hp = 100;								// ĳ���� HP ���� ����
	bool live = true;							// �ΰ����� ��� �ִ��� Ȯ��
	bool Game_Play = false;						// ���� �÷��� ������ Ȯ��
	char nickName[16];							// ĳ���� �г���
};

void init();														// ���� �ʱ�ȭ �� Ŭ���̾�Ʈ ������ �ʱ�ȭ
void err_quit( char *msg );									// ���� ������ ���� ��� ���ִ� �Լ�
void err_display( char *msg );								// ���� ������ ���� ��� ���ִ� �Լ�
int recvn( SOCKET s, char *buf, int len, int flags );		// Recv ������ ��� �ϴ� �Լ�

void Accept_Thread();											// Ŭ�� �����°� �޾��ִ� Thread
void ProcessPacket( int ci, char *packet );				// packet ó���� �ϴ� �κ�
DWORD WINAPI Work_Thread( LPVOID arg );			// Ŭ�󿡼� ���۵� Packet�� �о ó�����ش�.
void SendPacket( SOCKET sock, void *packet, int packet_size );		// ��Ŷ�� Ŭ�󿡰� ������ �ϴ� �Լ�

#endif