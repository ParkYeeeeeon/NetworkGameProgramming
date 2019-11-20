#pragma once
#include "protocol.h"

#pragma pack(1)
struct GameState {
	int packet_type = CS_PACKET_GAMESTATE;
	bool isstart;
	bool isend;
	float time;
	int score;
	int activate_thread;
};
#pragma pack()