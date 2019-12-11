#pragma once
#include "protocol.h"

#pragma pack(1)
struct Boss {
	int packet_type = CS_PACKET_BOSS;
	Location position;
	bool alive;
	int ani;
	int hp;
	int level;
	int kind;
	list<Bullet> bullet;
};
#pragma pack()

extern CImage Boss_image[1];
extern CImage Boss_warning_image;
extern CImage Boss_life_image;
extern CImage Boss_bullet; // º¸½º ÃÑ¾Ë
extern list<Bullet> bossBullet;

extern Boss boss;

void draw_boss(HDC hdc, int x, int y, int hp);
void draw_warning_ui(HDC hdc, int x, int y);
void change_boss_location();
void draw_bossbullet(HDC hdc);
void Bullet_Draw(HDC hdc, int x, int y, int bullettype);
void init_Boss_Image();
void add_boss_bullet();
void move_bossbullet();
void change_boss_bullet(list<Bullet>::iterator i);