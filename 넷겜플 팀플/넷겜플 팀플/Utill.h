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
};	// 좌표

struct Bullet {
	Location position;	// 총알의 위치
	int type;			// 어떤 객체의 총알인가
};

struct UI {
	Location position; // ui의 위치
	// 이미지 정보
};

void key_input();
void draw_ui();
void draw_map();
void Connect();
void Recv_Packet();
void Check_Packet();
void Send_Packet();