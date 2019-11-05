#pragma once
#include "Utill.h"

#pragma pack(1)
struct GameState {
	int packet_type = PACKET_GAMESTATE;
	bool isstart;
	bool isend;
	float time;
	int score;
	int activate_thread;
};
#pragma pack()