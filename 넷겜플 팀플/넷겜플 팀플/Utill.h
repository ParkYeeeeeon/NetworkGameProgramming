#pragma once
#include <stdio.h>
#include <Windows.h>
#include <vector>
#include <iostream>
#include <cstdlib> // 랜덤함수를 위한 헤더
#include <ctime>
#include <math.h>
#include <atlimage.h> // CImage

using namespace std;

#define display_start_x 500 // 출력지점
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
};	// 좌표

struct Bullet {
	Location position;	// 총알의 위치
	int type;			// 어떤 객체의 총알인가 0~2 : 몬스터, 3 : 중간보스, 4 : 보스, 5 : 플레이어, 6 : 다른 플레이어 
	int dir;				// 어떤 방향으로 갈지
	int bullet_type;	// 총알 그리기 타입 ( 뭔지 기억안남)
};


struct UI {					   // 0 : 플레이어1 , 1: 플레이어2
	Location hp_position[2];   // hp ui의 위치
	int hp[2];				   // hp 상태
	Location bomb_position[2]; // 폭탄 ui 위치
	int bomb[2];			   // 폭탄 갯수
	Location time_position;	   // 시간 ui 위치
	int time;				   // 시간 상태

	CImage hp_ui_img;		   // 이미지 정보
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