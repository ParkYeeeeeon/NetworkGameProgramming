#pragma once
#include "Utill.h"

#pragma pack(1)
struct Enemy {
	int packet_type = PACKET_ENEMY;
	Location position;
	int kind = 0;		// 움직임 최대값
	int ani = 0;
	int hp;
	bool alive = true;
	int level;
	vector<Bullet> bullet;
};
#pragma pack()


extern CImage Monster_image[10]; // 몬스터 캐릭터
extern CImage Monster_bullet[3]; // 몬스터 총알
extern Enemy mon1[MONSTER_COUNT];
extern Enemy mon2[MONSTER_COUNT];
extern Enemy mon3[MONSTER_COUNT];

void init_Monster_Image();
void init_Monster_Bullet_Image();
void Monster_Draw(HDC hdc, int x, int y, int Kind, int ani, int hp);
void Bullet_Draw(HDC hdc, int x, int y, int Kind);
void draw_enemy(HDC hdc);
void draw_enemybullet(HDC hdc);
void change_enemy_ani(int monsterKind, int monster_id);
void change_enemy_location(int monsterkind, int monster_id);
void change_enemy_bullet(int monsterKind, int monster_id);
