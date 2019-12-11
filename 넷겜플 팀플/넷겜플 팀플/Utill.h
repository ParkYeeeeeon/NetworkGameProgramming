#pragma once
#include <stdio.h>
#include <Windows.h>
#include <vector>
#include <list>
#include <iostream>
#include <cstdlib> // �����Լ��� ���� ���
#include <ctime>
#include <math.h>
#include <atlimage.h> // CImage

using namespace std;

#define display_start_x 500 // �������
#define display_end_x 1500
#define display_start_y 20
#define display_end_y 630

#define MAX_BUFSIZE 1024

#define MONSTER_COUNT 20

#define LIMIT_PLAYER 2	// �ִ� ���� �� �ִ� �÷��̾� ����
#define PLAYER_OTHER 0
#define PLAYER_ME 1

#define PLAYER_SIZE 50
#define PLAYER_BULLET_SIZE 20

#define MONSTER_SIZE_X 20   
#define MONSTER_SIZE_Y 16
#define MONSTER_BULLET_SIZE 50

struct Location {
	int x;
	int y;
};	// ��ǥ

struct Bullet {
	Location position;	// �Ѿ��� ��ġ
	int type;			// � ��ü�� �Ѿ��ΰ� 0~2 : ����, 3 : �߰�����, 4 : ����, 5 : �÷��̾�, 6 : �ٸ� �÷��̾� 
	int dir;				// � �������� ����
	int bullet_type;	// �Ѿ� �׸��� Ÿ�� ( ���� ���ȳ�)
	bool draw;			// �׸��� ����
};


struct UI {					   // 0 : �÷��̾�1 , 1: �÷��̾�2
	Location hp_position[2];   // hp ui�� ��ġ
	int hp[2];				   // hp ����
	Location bomb_position[2]; // ��ź ui ��ġ
	int bomb[2];			   // ��ź ����
	Location time_position;	   // �ð� ui ��ġ
	int time;				   // �ð� ����

	CImage hp_ui_img;		   // �̹��� ����
	CImage bomb_ui_img;        
};

void key_input(int key);
void init_ui(UI& ui);
void draw_ui(HDC hdc, UI& ui);
void draw_map(HDC hdc, CImage& mapimg);
void draw_Timer(HDC hdc, int time);
void draw_number(HDC hdc, int num, int x, int y);
void draw_mainImage(HDC hdc, CImage& mapimg);
void draw_buttonImage(HDC hdc, CImage& buttonimg,int x,int y);
void change_image(CImage& startimg, CImage& readyimg); //��ư Ŭ�� �� �̹��� ����
void set_number();
void Connect();
void Recv_Packet();
void Check_Packet();
void Send_Packet();
int get_distance(Location l1, Location l2);
bool crash_check(int myX, int myY, int uX, int uY, int uType);

void init_sock();
void err_quit(char *msg);									// ���� ������ ���� ��� ���ִ� �Լ�
void err_display(char *msg);								// ���� ������ ���� ��� ���ִ� �Լ�
int recvn(SOCKET s, char *buf, int len, int flags);		// Recv ������ ��� �ϴ� �Լ�
void SendPacket(SOCKET sock, void *packet, int packet_size);		// Packet ����
DWORD WINAPI Read_Thread(LPVOID arg);
void ProcessPacket(int ci, char *packet);
void reconnect_socket(SOCKET &sock);