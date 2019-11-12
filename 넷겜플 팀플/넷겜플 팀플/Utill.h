#pragma once
#include <stdio.h>
#include <Windows.h>
#include <vector>
#include <iostream>
#include <cstdlib> // 랜덤함수를 위한 헤더
#include <ctime>
#include <atlimage.h> // CImage

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


struct UI {					   // 0 : 플레이어1 , 1: 플레이어2
	Location hp_position[2];   // hp ui의 위치
	int hp[2];				   // hp 상태
	Location bomb_position[2]; // 폭탄 ui 위치
	int bomb[2];			   // 폭탄 갯수
	Location time_position;	   // 시간 ui 위치
	int time;				   // 시간 상태
							   // 이미지 정보
};

void key_input(int key);
void draw_ui(UI ui);
void draw_map();
void Connect();
void Recv_Packet();
void Check_Packet();
void Send_Packet();