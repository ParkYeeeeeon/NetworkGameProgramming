#pragma once
#include <stdio.h>
#include <Windows.h>
#include <vector>
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

#define MONSTER_COUNT 5

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
void Connect();
void Recv_Packet();
void Check_Packet();
void Send_Packet();
int get_distance(Location l1, Location l2);