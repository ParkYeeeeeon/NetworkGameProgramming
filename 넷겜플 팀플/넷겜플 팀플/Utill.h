#pragma once
#include <stdio.h>
#include <Windows.h>
#include <vector>
#include <iostream>
#include <cstdlib> // 랜덤함수를 위한 헤더
#include <ctime>

using namespace std;

#define PACKET_PLAYER 0
#define PACKET_ENEMY 1
#define PACKET_BOSS 2
#define PACKET_GAMESTATE 3

#define display_start_x 300 // 출력지점
#define display_end_x 1100
#define display_start_y 300
#define display_end_y 900


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