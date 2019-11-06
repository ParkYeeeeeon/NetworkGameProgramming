#pragma once
#include "Utill.h"

#pragma pack(1)
struct Enemy {
	int packet_type = PACKET_ENEMY;
	Location position;
	int hp;
	bool alive = true;
	int level;
	vector<Bullet> bullet;
};
#pragma pack()

CImage Monster_image[10]; // 몬스터 캐릭터
HDC hdc, mem0dc, mem1dc;

void init_Image() {
	Monster_image[0].Load("Image\\Monster\\mon1.png");
}

void Monster_Draw(HDC hdc, int x, int y, int Kind, int hp) {
	SetTextColor(hdc, RGB(0, 0, 255));
	SetBkMode(hdc, TRANSPARENT);
	Monster_image[Kind].Draw(hdc, 0 + (x * 40), 0 + (y * 40), 40, 40, 0, 0, Monster_image[Kind].GetWidth(), Monster_image[Kind].GetHeight());
}

void draw_enemy();
void draw_enemybullet();
