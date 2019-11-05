#pragma once
#include "Utill.h"

#pragma pack(1)
struct Enemy {
	int packet_type = PACKET_ENEMY;
	Location postion;
	int hp;
	int level;
	vector<Bullet> bullet;
};
#pragma pack()

void draw_enemy();
void draw_enemybullet();
