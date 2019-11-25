#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include <vector>	// [서버에서는 제거]

#define SERVERPORT 9000

// 기본 구조체 [서버에서는 제거]
struct Location {
	int x;
	int y;
};	// 좌표

struct Bullet {
	Location position;	// 총알의 위치
	int type;			// 어떤 객체의 총알인가
	int dir;				// 어떤 방향으로 갈지
	int bullet_type;	// 총알 그리기 타입
};
// 기본 구조체 [서버에서는 제거]

// 클라 -> 서버
#define CS_PACKET_PLAYER 1
#define CS_PACKET_ENEMY 2
#define CS_PACKET_BOSS 3
#define CS_PACKET_GAMESTATE 4
#define CS_PACKET_DIR 5
#define CS_PACKET_ATTACK 6

#define VK_UP_UP 100
#define VK_DOWN_UP 101
#define VK_UP_DOWN 200
#define VK_DOWN_DOWN 201
#define VK_UP_LEFT 300
#define VK_DOWN_LEFT 301
#define VK_UP_RIGHT 400
#define VK_DOWN_RIGHT 401
#define VK_UP_SPACE 500
#define VK_DOWN_SPACE 501

// 서버 -> 클라
#define SC_PACKET_CINO 1
#define SC_PACKET_DIR 2
#define SC_PACKET_ATTACK 3
#define SC_PACKET_BOMB 4
#define SC_PACKET_PLAYER_0 5
#define SC_PACKET_PLAYER_1 6

// 패킷 구조체 정보
struct cs_packet_dir {
	int type;
	//----------------
	int dir;
};

struct cs_packet_player {
	int type;
	//----------------	
	Location position;
	int hp;
	int bullet_damage;
	float attack_speed;
	int bomb;
	std::vector<Bullet> b;
};

struct cs_packet_enemy {
	int type;
	//----------------
	Location position;
	int level;
	int hp;
	std::vector<Bullet> b;

};

struct cs_packet_boss {
	int type;
	//----------------
	Location position;
	int level;
	int hp;
	std::vector<Bullet> b;

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

struct sc_packet_cino {
	int type;
	//----------------
	int no;
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