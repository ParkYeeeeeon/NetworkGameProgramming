#pragma once
//#include "Utill.h"
#include "protocol.h"

#pragma pack(1) 
struct Player {
	int packet_type = CS_PACKET_PLAYER;
	bool control;
	Location position;
	int hp;
	int bullet_damage;
	int attack_speed;
	int bomb;
	vector<Bullet> bullet;
	bool fire = false;	// 총알 쐈는가 (space바를 눌렀는가)
	bool skil_activate;
	CImage player_img;
	CImage player_up_img;
	CImage player_down_img;
	CImage bullet_img;

	int moveX = 0; // 0 : 정지 , 1 : 증가 , 2 : 감소
	int moveY = 0; // 0 : 정지 , 1 : 증가 , 2 : 감소
};
#pragma pack()

void set_player(Player player[2]);
void draw_player(HDC hdc, Player player[2]);
void add_player_bullet(Player player[2]);
void draw_playerbullet(HDC hdc, Player player[2]);
void add_bullet_position(Player player[2]);