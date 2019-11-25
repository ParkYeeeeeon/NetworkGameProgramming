#include "Player.h"

void set_player(Player player[2]) {
	player[0].player_img.Load("Image\\Player\\Player1.png");
	player[0].player_up_img.Load("Image\\Player\\Player1_up.png");
	player[0].player_down_img.Load("Image\\Player\\Player1_down.png");
	player[0].bullet_img.Load("Image\\Player\\Player1_Bullet.png");

	player[0].position.y = 100;

	player[1].player_img.Load("Image\\Player\\Player2.png");
	player[1].player_up_img.Load("Image\\Player\\Player2_up.png");
	player[1].player_down_img.Load("Image\\Player\\Player2_down.png");
	player[1].bullet_img.Load("Image\\Player\\Player2_Bullet.png");

	player[1].position.y = 400;
}

void draw_player(HDC hdc, Player player[2]) {
	// 플레이어 그리기
	for (int i = 0; i < 2; ++i) {
			SetTextColor(hdc, RGB(0, 0, 255));
			SetBkMode(hdc, TRANSPARENT);
			switch (player[i].moveY) {
			case 0:
				player[i].player_img.Draw(hdc, player[i].position.x, player[i].position.y, 50, 50);
				break;
			case 1:
				player[i].player_down_img.Draw(hdc, player[i].position.x, player[i].position.y, 50, 50);
				break;
			case 2:
				player[i].player_up_img.Draw(hdc, player[i].position.x, player[i].position.y, 50, 50);
				break;
		}
	}
}

void add_player_bullet(Player player[2]) {
	for (int i = 0; i < 2; ++i) {
		if (player[i].fire == true) {
			Bullet tmp;
			tmp.position.x = player[i].position.x + 55;
			tmp.position.y = player[i].position.y + 20;
			tmp.type = 5;
			tmp.dir = 0;
			tmp.bullet_type = 0;
			player[i].bullet.emplace_back(tmp);
		}
	}
}

void draw_playerbullet(HDC hdc, Player player[2]) {
	// 플레이어 총알 그리기
	for (int i = 0; i < 2; ++i) {
		if (player[i].bullet.size() > 0) {
			for (vector<Bullet>::iterator j = player[i].bullet.begin(); j < player[i].bullet.end(); ++j)
				player[i].bullet_img.Draw(hdc, j->position.x, j->position.y, 20, 20);
		}
	}
}

void add_bullet_position(Player player[2]) {
	for (int i = 0; i < 2; ++i) {
		for (vector<Bullet>::iterator j = player[i].bullet.begin(); j < player[i].bullet.end(); ++j) {
			j->position.x += 10;
		}
	}
}
