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

void draw_player(HDC hdc, Player player[2], int ci, skill sk) {
	// 플레이어 그리기
	SetTextColor(hdc, RGB(0, 0, 255));
	SetBkMode(hdc, TRANSPARENT);
	switch (player[ci].moveY) {
	case 0:
		player[ci].player_img.Draw(hdc, player[ci].position.x, player[ci].position.y, 50, 50);
		break;
	case 1:
		player[ci].player_down_img.Draw(hdc, player[ci].position.x, player[ci].position.y, 50, 50);
		break;
	case 2:
		player[ci].player_up_img.Draw(hdc, player[ci].position.x, player[ci].position.y, 50, 50);
		break;
	}

	if (player[0].skil_activate == true) {
		draw_skill(hdc, sk, sk.position.x, sk.position.y, sk.ani);
	}
}

void add_player_bullet(Player player[2], Bullet recv_bullet[2][200]) {
	for (int i = 0; i < 2; ++i) {
		for (int j = 0; j < 200; ++j) {
			if (recv_bullet[i][j].draw) {
				Bullet tmp;
				tmp.position.x = recv_bullet[i][j].position.x;
				tmp.position.y = recv_bullet[i][j].position.y;
				tmp.type = 5;
				tmp.dir = 0;
				tmp.dir = true;
				player[i].bullet.emplace_back(tmp);
			}
			
		}
	}
}

void draw_playerbullet(HDC hdc, Player player[2]) {
	// 플레이어 총알 그리기
	for (int i = 0; i < LIMIT_PLAYER; ++i) {
		// 플레이어가 접속 했을 경우에만 처리
		if (player[i].connect == true) {
			if (player[i].bullet.size() > 0) {

				for (auto bullet : player[i].bullet) {
					if (bullet.draw == false)
						continue;
					//Rectangle(hdc, bullet.position.x, bullet.position.y, bullet.position.x + 20, bullet.position.y + 20);
					player[i].bullet_img.Draw(hdc, bullet.position.x, bullet.position.y, 20, 20);
				}
			}
		}
	}
}

//void add_bullet_position(Player player[2]) {
//	for (int i = 0; i < 2; ++i) {
//		for (vector<Bullet>::iterator j = player[i].bullet.begin(); j < player[i].bullet.end(); ++j) {
//			j->position.x += 10;
//		}
//	}
//}

void set_skill_image(skill& sk) {
	sk.skill_image.Load("Image\\Player\\player_skill.png");	
}

void get_skill_position(Player p[2], skill& sk) {
	Location skill_position[2];
	Location result;

	for (int i = 0; i < 2; ++i) {
		skill_position[i] = p[i].position;
		skill_position[i].x += PLAYER_SIZE / 2;
		skill_position[i].y += PLAYER_SIZE / 2;
	}

	result.x = (skill_position[0].x + skill_position[1].x) / 2;
	result.y = (skill_position[0].y + skill_position[1].y) / 2;
	
	sk.position = result;
}

void change_skill_ani(skill& sk) {
	sk.ani += 1;
	if (sk.ani >= 9)
		sk.ani = 0;
}

void draw_skill(HDC hdc, skill sk, int x, int y, int ani) {
	SetTextColor(hdc, RGB(0, 0, 255));
	SetBkMode(hdc, TRANSPARENT);
	sk.skill_image.Draw(hdc, sk.position.x, sk.position.y, 50, 50, 0 + (ani * 100), 0, 50, 50);
}