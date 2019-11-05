#pragma once
#include "Utill.h"

#pragma pack(1)
struct Boss {
	int packet_type = PACKET_BOSS;
	Location position;
	int hp;
	int level;
	vector<Bullet> bullet;
};
#pragma pack()

void draw_boss();
void draw_bossbullet();