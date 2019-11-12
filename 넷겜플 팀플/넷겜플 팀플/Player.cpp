#include "Player.h"

void set_player(Player player[2], Player packet[2]) {
	for (int i = 0; i < 2; ++i) {
		player[i] = packet[i];
	}
}

void draw_player(Player& p) {
	// 플레이어 그리기
}

void draw_playerbullet(Player& p) {
	// 플레이어 총알 그리기
}