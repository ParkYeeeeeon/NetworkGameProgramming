#ifndef __OBJECT_H__
#define __OBJECT_H__

#include "protocol.h"

#define PLAYER_SIZE 50
#define PLAYER_BULLET_SIZE 20

#define MONSTER_COUNT 5

#define MONSTER_SIZE_X 20   
#define MONSTER_SIZE_Y 16
#define MONSTER_BULLET_SIZE 50


#define PLAYER_BULLET_LIMIT 100

#define display_start_x 500 // 출력지점
#define display_end_x 1500
#define display_start_y 20
#define display_end_y 630

struct Player {
	//SOCKET sock;	// 외부 함수에서 socket 통신을 위하여 가지고 있는다.
	Location position;
	int hp;
	int bullet_damage;
	int attack_speed;
	int bomb;
	std::vector<Bullet> bullet;
	bool skil_activate;

	int moveX = 0; // 0 : 정지 , 1 : 증가 , 2 : 감소
	int moveY = 0; // 0 : 정지 , 1 : 증가 , 2 : 감소
};

struct Enemy {
	Location position;
	int kind = 0;		// 몬스터 종류
	int ani = 0;
	int hp;
	bool alive = false;
	int level;
};

struct Boss {
	Location position;
	int hp;
	int level;
	std::vector<Bullet> bullet;
};

struct GameState {
	bool isstart;
	bool isend;
	float time;
	int score;
	int activate_thread;
};

struct UI {					   // 0 : 플레이어1 , 1: 플레이어2
	int hp[2];				   // hp 상태
	int bomb[2];			   // 폭탄 갯수
	int time;				   // 시간 상태

};

void set_player(Player player[2]);

//void add_player_bullet(CLIENT player[2]);
//void add_bullet_position(CLIENT player[2]);

#endif