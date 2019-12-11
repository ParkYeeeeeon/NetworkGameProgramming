#pragma once
//#include "Utill.h"
#include "protocol.h"

#pragma pack(1) 
struct Player {
	int packet_type = CS_PACKET_PLAYER;
	bool connect;	// 연결 여부
	int dirX = 0;
	int dirY = 0;
	bool control;
	Location position;
	int hp;
	int bullet_damage;
	int attack_speed;
	int item;
	list<Bullet> bullet;
	bool fire = false;	// 총알 쐈는가 (space바를 눌렀는가)
	bool skil_activate;
	bool ready = false;
	CImage player_img;
	CImage player_up_img;
	CImage player_down_img;
	CImage bullet_img;

	int moveX = 0; // 0 : 정지 , 1 : 증가 , 2 : 감소
	int moveY = 0; // 0 : 정지 , 1 : 증가 , 2 : 감소
};
#pragma pack()

struct skill {
	CImage skill_image;
	Location position;
	int ani = 0;
};

void set_player(Player player[2]);
void draw_player(HDC hdc, Player player[2], int ci, skill sk);
void add_player_bullet(Player player[2], Bullet recv_bullet[2][200]);
void draw_playerbullet(HDC hdc, Player player[2]);
//void add_bullet_position(Player player[2]);

void set_skill_image(skill& sk);
void get_skill_position(Player p[2], skill& sk);
void change_skill_ani(skill& sk);
void draw_skill(HDC hdc, skill sk, int x, int y, int ani);