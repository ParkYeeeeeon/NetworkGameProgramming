#ifndef __OBJECT_H__
#define __OBJECT_H__

#include "protocol.h"

struct Player {
	//SOCKET sock;	// 외부 함수에서 socket 통신을 위하여 가지고 있는다.
	Location position;
	int hp;
	int bullet_damage;
	int attack_speed;
	int bomb;
	std::vector<Bullet> bullet;
	bool fire = false;	// 총알 쐈는가 (space바를 눌렀는가)
	bool skil_activate;

	int moveX = 0; // 0 : 정지 , 1 : 증가 , 2 : 감소
	int moveY = 0; // 0 : 정지 , 1 : 증가 , 2 : 감소
};

struct Enemy {
	Location position;
	int kind = 0;		// 움직임 최대값
	int ani = 0;
	int hp;
	bool alive = true;
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
void add_player_bullet(Player player[2]);
void add_bullet_position(Player player[2]);
#endif