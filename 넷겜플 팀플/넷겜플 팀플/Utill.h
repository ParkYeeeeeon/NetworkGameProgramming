#pragma once
#include <stdio.h>
#include <Windows.h>
#include <vector>
#include <iostream>
#include <cstdlib> // �����Լ��� ���� ���
#include <ctime>
#include <atlimage.h> // CImage

using namespace std;

#define PACKET_PLAYER 0
#define PACKET_ENEMY 1
#define PACKET_BOSS 2
#define PACKET_GAMESTATE 3

#define display_start_x 300 // �������
#define display_end_x 1100
#define display_start_y 300
#define display_end_y 900


struct Location {
	int x;
	int y;
};	// ��ǥ

struct Bullet {
	Location position;	// �Ѿ��� ��ġ
	int type;			// � ��ü�� �Ѿ��ΰ�
};


struct UI {					   // 0 : �÷��̾�1 , 1: �÷��̾�2
	Location hp_position[2];   // hp ui�� ��ġ
	int hp[2];				   // hp ����
	Location bomb_position[2]; // ��ź ui ��ġ
	int bomb[2];			   // ��ź ����
	Location time_position;	   // �ð� ui ��ġ
	int time;				   // �ð� ����
							   // �̹��� ����
};

void key_input(int key);
void draw_ui(UI ui);
void draw_map();
void Connect();
void Recv_Packet();
void Check_Packet();
void Send_Packet();