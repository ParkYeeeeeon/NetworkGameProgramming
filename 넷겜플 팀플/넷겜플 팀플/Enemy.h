#pragma once
#include "protocol.h"

#pragma pack(1)
struct Enemy {
	int packet_type = CS_PACKET_ENEMY;
	Location position;
	int kind = 0;		// ���� ����
	int ani = 0;
	int hp;
	bool alive = false;
	int level;
};
#pragma pack()

extern list<Bullet> bullet;
extern CImage Monster_image[10]; // ���� ĳ����
extern CImage Monster_bullet[3]; // ���� �Ѿ�
extern Enemy monster[MONSTER_COUNT];
//extern Enemy mon1[MONSTER_COUNT];
//extern Enemy mon2[MONSTER_COUNT];
//extern Enemy mon3[MONSTER_COUNT];

void init_Monster_Image();
void init_Monster_Bullet_Image();
void Monster_Draw(HDC hdc, int x, int y, int Kind, int ani, int hp);
void Bullet_Draw(HDC hdc, int x, int y, int Kind, int bullettype);
void draw_enemy(HDC hdc);
void draw_enemybullet(HDC hdc);
void change_enemy_ani(int monsterKind, int monster_id);
void change_enemy_location(int monsterkind, int monster_id);
void change_enemy_bullet(std::list<Bullet>::iterator i );
void add_enemy_bullet();
void draw_bullet_status(HDC mem0dc);
void move_enemybullet();