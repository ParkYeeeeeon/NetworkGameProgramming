#pragma once
#include "Utill.h"

#pragma pack(1) 
struct Player {
	int packet_type = PACKET_PLAYER;
	Location position;
	int hp;
	int bullet_damage;
	int attack_speed;
	int bomb;
	vector<Bullet> bullet;
	bool skil_activate;
};
#pragma pack()

void set_player(Player player[2], Player packet[2]);
void draw_player(Player& p);
void draw_playerbullet(Player& p);