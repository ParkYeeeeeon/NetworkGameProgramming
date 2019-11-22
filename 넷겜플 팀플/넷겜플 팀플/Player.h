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
	bool fire = false;	// �Ѿ� ���°� (space�ٸ� �����°�)
	bool skil_activate;
	CImage player_img;
	CImage player_up_img;
	CImage player_down_img;
	CImage bullet_img;

	int moveX = 0; // 0 : ���� , 1 : ���� , 2 : ����
	int moveY = 0; // 0 : ���� , 1 : ���� , 2 : ����
};
#pragma pack()

void set_player(Player player[2]);
void draw_player(HDC hdc, Player player[2]);
void add_player_bullet(Player player[2]);
void draw_playerbullet(HDC hdc, Player player[2]);
void add_bullet_position(Player player[2]);