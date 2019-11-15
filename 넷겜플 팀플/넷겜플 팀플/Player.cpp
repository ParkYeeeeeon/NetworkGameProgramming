#include "Player.h"

void set_player(Player player[2], Player packet[2]) {
	for (int i = 0; i < 2; ++i) {
		player[i] = packet[i];
	}
}

void draw_player(HDC hdc, Player player[2]) {
	// 플레이어 그리기
	for (int i = 0; i < 2; ++i) {
		if (player[i].control == PLAYER_ME) {
			SetTextColor(hdc, RGB(0, 0, 255));
			SetBkMode(hdc, TRANSPARENT);
			switch (player[i].moveY) {
			case 0:
				player[i].player_img.Draw(hdc, 50 + player[i].position.x, 200 + player[i].position.y, 50, 50);
				break;
			case 1:
				player[i].player_down_img.Draw(hdc, 50 + player[i].position.x, 200 + player[i].position.y, 50, 50);
				break;
			case 2:
				player[i].player_up_img.Draw(hdc, 50 + player[i].position.x, 200 + player[i].position.y, 50, 50);
				break;
			}
		}
	}
}

void draw_playerbullet(Player& p) {
	// 플레이어 총알 그리기
}