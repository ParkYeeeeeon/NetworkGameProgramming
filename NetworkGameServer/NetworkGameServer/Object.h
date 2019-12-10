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

#define display_start_x 500 // �������
#define display_end_x 1500
#define display_start_y 20
#define display_end_y 630

struct Player {
	//SOCKET sock;	// �ܺ� �Լ����� socket ����� ���Ͽ� ������ �ִ´�.
	Location position;
	int hp;
	int bullet_damage;
	int attack_speed;
	int bomb;
	std::vector<Bullet> bullet;
	bool skil_activate;

	int moveX = 0; // 0 : ���� , 1 : ���� , 2 : ����
	int moveY = 0; // 0 : ���� , 1 : ���� , 2 : ����
};

struct Enemy {
	Location position;
	int kind = 0;		// ���� ����
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

struct UI {					   // 0 : �÷��̾�1 , 1: �÷��̾�2
	int hp[2];				   // hp ����
	int bomb[2];			   // ��ź ����
	int time;				   // �ð� ����

};

void set_player(Player player[2]);

//void add_player_bullet(CLIENT player[2]);
//void add_bullet_position(CLIENT player[2]);

#endif