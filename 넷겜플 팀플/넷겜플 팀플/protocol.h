#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include "Utill.h"

#define SERVERIP "127.0.0.1"
#define SERVERPORT 9000

// 클라 -> 서버
#define CS_PACKET_PLAYER 1
#define CS_PACKET_ENEMY 2
#define CS_PACKET_BOSS 3
#define CS_PACKET_GAMESTATE 4
#define CS_PACKET_DIR 5
#define CS_PACKET_ATTACK 6
#define CS_PACKET_RECONNECT 7

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
#define SC_PACKET_CONNECT 7
#define SC_PACKET_TIME 8
#define SC_PACKET_BULLET 9
#define SC_PACKET_MONSTER_LOCATION 10
#define SC_PACKET_MONSTER_BULLET 11
#define SC_PACKET_DELETE_PLAYER_BULLET 12

// [클라이언트, 서버] define숫자에서 +1 을 하여 잘못된 패킷을 방지 하기 위하여 작성
// 패킷을 하나 추가 할 떄마다 아래 값도 증가를 해줘야 한다.
#define LIMIT_PACKET_CLIENT_NO 13
#define LIMIT_PACKET_SERVER_NO 9

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

struct cs_packet_attack {
	int type;
	//----------------
	bool attack;
	int damage;
};

struct sc_packet_cino {
	int type;
	//----------------
	int no;
};

struct sc_packet_connect {
	int type;
	//----------------
	int no;
	bool connect;
};

struct sc_packet_location {
	int type;
	//----------------
	int ci;
	int x;
	int y;
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

struct sc_packet_time {
	int type;
	//----------------
	int progress_time;
};

struct sc_packet_bullet {
	int type;
	//----------------
	int no;
	Bullet bullet;
};

struct sc_packet_monster_bullet {
	int type;
	//----------------
	std::vector<Bullet> bullet;
};

struct sc_packet_monster_location {
	int type;
	//----------------
	int no;	// 몬스터 번호
	Location position;
	int kind;		// 몬스터 종류
	int ani;
	int hp;
	bool alive;
	int level;
};

struct sc_packet_delete_player_bullet {
	int type;
	//---------------------
	int ci;  // 어떤 플레이어의 총알을 지울지
	int num; // bullet 벡터에서 몇번째 원소인지
};

#endif