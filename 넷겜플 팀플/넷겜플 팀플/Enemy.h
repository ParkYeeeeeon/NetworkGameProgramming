#pragma once
#include "Utill.h"

#pragma pack(1)
struct Enemy {
	int packet_type = PACKET_ENEMY;
	Location position;
	int kind = 0;		// 움직임 최대값
	int hp;
	bool alive = true;
	int level;
	vector<Bullet> bullet;
};
#pragma pack()


extern CImage Monster_image[10]; // 몬스터 캐릭터
extern Enemy mon1[100];
extern Enemy mon2[100];
extern Enemy mon3[100];

void init_Monster_Image();
void Monster_Draw(HDC hdc, int x, int y, int Kind, int ani, int hp);
void draw_enemy(HDC hdc);
void draw_enemybullet();
