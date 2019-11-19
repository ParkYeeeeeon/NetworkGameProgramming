#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include "Utill.h"


#define SERVERPORT 9000

// 클라 -> 서버
#define CS_PACKET_PLAYER 1
#define CS_PACKET_ENEMY 2
#define CS_PACKET_BOSS 3
#define CS_PACKET_GAMESTATE 4


// 서버 -> 클라
#define SC_PACKET_DIR 1
#define SC_PACKET_ATTACK 2
#define SC_PACKET_BOMB 3

// 패킷 구조체 정보
struct cs_packet_player {
	int type;
	//----------------	
	Location position;
	int hp;
	int bullet_damage;
	float attack_speed;
	int bomb;
	vector<Bullet> b;
};

struct cs_packet_enemy {
	int type;
	//----------------
	Location position;
	int level;
	int hp;
	vector<Bullet> b;

};

struct cs_packet_boss {
	int type;
	//----------------
	Location position;
	int level;
	int hp;
	vector<Bullet> b;

};

struct cs_packet_gamestate {
	int type;
	//----------------
	bool isstart;
	bool isend;
	float time;
	int score;
	int activated_thread;


};

struct sc_packet_dir {
	int type;
	//----------------
	int dir;
};

struct sc_packet_attack {
	int type;
	//----------------
	bool attack;
	int damage;
};

struct sc_packet_bomb {
	int type;
	//----------------
	bool bombuse;
};




#endif