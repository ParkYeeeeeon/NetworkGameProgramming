#pragma once
#include <stdio.h>
#include <Windows.h>
#include <vector>
#include <iostream>

using namespace std;

#define PACKET_PLAYER 0
#define PACKET_ENEMY 1
#define PACKET_BOSS 2
#define PACKET_GAMESTATE 3

struct Location {
	int x;
	int y;
};	// ��ǥ

struct Bullet {
	Location position;	// �Ѿ��� ��ġ
	int type;			// � ��ü�� �Ѿ��ΰ�
};

struct UI {
	Location position; // ui�� ��ġ
	// �̹��� ����
};

void key_input();
void draw_ui();
void draw_map();
void Connect();
void Recv_Packet();
void Check_Packet();
void Send_Packet();