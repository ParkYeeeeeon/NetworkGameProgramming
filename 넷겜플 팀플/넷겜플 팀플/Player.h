#pragma once
#include "Utill.h"

#pragma pack(1) 
struct Player {
	int packet_type = PACKET_PLAYER;
	bool control;
	Location position;
	int hp;
	int bullet_damage;
	int attack_speed;
	int bomb;
	vector<Bullet> bullet;
	bool skil_activate;
	CImage player_img;
	CImage player_up_img;
	CImage player_down_img;

	int moveX = 0; // 0 : 정지 , 1 : 증가 , 2 : 감소
	int moveY = 0; // 0 : 정지 , 1 : 증가 , 2 : 감소
};
#pragma pack()

void set_player(Player player[2], Player packet[2]);
void draw_player(HDC hdc, Player player[2]);
void draw_playerbullet(Player& p);