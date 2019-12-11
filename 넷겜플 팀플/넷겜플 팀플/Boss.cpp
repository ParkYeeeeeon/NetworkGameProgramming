#include "Boss.h"

CImage Boss_image[1];
CImage Boss_warning_image;
CImage Boss_life_image;
CImage Boss_bullet;

Boss boss;
list<Bullet> bossBullet;

void init_Boss_Image() {
	Boss_image[0].Load("Image\\Monster\\boss.png");
	Boss_warning_image.Load("Image\\UI\\warning.png");
	Boss_life_image.Load("Image\\UI\\LIFE.png");
	Boss_bullet.Load("Image\\Monster\\적총알보스.png");
	srand((unsigned int)time(NULL));
}

void draw_boss(HDC hdc, int x, int y, int hp) {
	// 보스 그리기
	SetTextColor(hdc, RGB(0, 0, 255));
	SetBkMode(hdc, TRANSPARENT);
	int monsterX = 452, monsterY = 320;
	int lifeX = 960, lifeY = 960;
	int draw_lifeX = x, draw_lifeY = y - 35;

	// HP 그리기 hp 100당 하트 1개씩
	for (int i = 0; i < hp; ) {
		Boss_life_image.Draw(hdc, 0 + (draw_lifeX * 1), 0 + (draw_lifeY * 1), 30, 30, 0, 0, lifeX, lifeY);
		draw_lifeX += 31;
		i += 100;
	}

	// 보스 이미지 그리기
	Boss_image[0].Draw(hdc, 0 + (x * 1), 0 + (y * 1), 300, 230, 0, 0, monsterX, monsterY);
}

void draw_warning_ui(HDC hdc, int x, int y) {
	SetTextColor(hdc, RGB(0, 0, 255));
	SetBkMode(hdc, TRANSPARENT);
	int warningX = 640, warningY = 298;

	Boss_warning_image.Draw(hdc, 0 + (x - (warningX / 2)), 0 + (y - (warningY / 2)), 640, 298, 0, 0, warningX, warningY);
}

void change_boss_location() {
	if (boss.ani == 0) {
		boss.position.y += 4;
	}
	else {
		boss.position.y -= 4;
	}
	if (boss.position.y >= 550) {
		boss.ani = 1;
	}
	else if (boss.position.y <= -100) {
		boss.ani = 0;
	}
}

void draw_bossbullet(HDC hdc) {
	for (list<Bullet>::iterator i = bossBullet.begin(); i != bossBullet.end();)
	{
		if (i->draw == false) {
			i = bossBullet.erase(i);
			continue;
		}
		// 총알이 영역 밖으로 나갔을 경우 삭제
		if ((i->position.x <= 0 || i->position.y <= 0 || i->position.y >= 720)) {
			// 범위를 벗어나면 삭제를 해준다.
			i = bossBullet.erase(i);
		}
		else {
			Bullet_Draw(hdc, i->position.x, i->position.y, i->bullet_type);
			++i;
		}
	}
}

void Bullet_Draw(HDC hdc, int x, int y, int bullettype) {
	SetTextColor(hdc, RGB(0, 0, 255));
	SetBkMode(hdc, TRANSPARENT);
	int bulletX = 46, bulletY = 46;

	if (bullettype == 1) {
		// 총알을 가로로 길게 만들기
		Boss_bullet.Draw(hdc, 0 + (x * 1), 0 + (y * 1), 20, 10, 0, 0, bulletX, bulletY);
	}
	else {
		Boss_bullet.Draw(hdc, 0 + (x * 1), 0 + (y * 1), 10, 10, 0, 0, bulletX, bulletY);
	}
}

void add_boss_bullet() {
	switch (boss.kind) {
	case 0:
	{
		Bullet tmp_bullet;
		tmp_bullet.position.x = boss.position.x - 20;
		tmp_bullet.position.y = boss.position.y + 100;
		tmp_bullet.type = 0;
		tmp_bullet.dir = 0;
		tmp_bullet.bullet_type = 0;
		tmp_bullet.dir = 0;
		bossBullet.push_back(tmp_bullet);
	}
	break;

	case 1:
	{
		Bullet tmp_bullet;
		tmp_bullet.position.x = boss.position.x - 20;
		tmp_bullet.position.y = boss.position.y + 90;
		tmp_bullet.type = 0;
		tmp_bullet.dir = 0;
		tmp_bullet.bullet_type = 1;
		tmp_bullet.dir = 0;
		bossBullet.push_back(tmp_bullet);

		tmp_bullet.position.x = boss.position.x - 20;
		tmp_bullet.position.y = boss.position.y + 130;
		tmp_bullet.type = 0;
		tmp_bullet.dir = 0;
		tmp_bullet.bullet_type = 1;
		tmp_bullet.dir = 0;
		bossBullet.push_back(tmp_bullet);
	}
	break;

	case 2:
	{
		for (int j = 0; j < 3; ++j) {
			Bullet tmp_bullet;
			tmp_bullet.position.x = boss.position.x - 20;
			tmp_bullet.position.y = boss.position.y + 100;
			tmp_bullet.type = 0;
			tmp_bullet.dir = 0;
			tmp_bullet.bullet_type = 0;
			tmp_bullet.dir = j;	// 직진 // 왼쪽 위로 // 왼쪽 아래
			bossBullet.push_back(tmp_bullet);
		}
	}
	break;

	}
}

void move_bossbullet() {
	for (list<Bullet>::iterator i = bossBullet.begin(); i != bossBullet.end();)
	{
		// 총알이 영역 밖으로 나갔을 경우 삭제
		if ((i->position.x <= 0 || i->position.y <= 0 || i->position.y >= 720) || i->draw == false) {
			// 범위를 벗어나면 삭제를 해준다.
			i = bossBullet.erase(i);
		}
		else {
			change_boss_bullet(i);
			++i;
		}
	}
}

void change_boss_bullet(list<Bullet>::iterator i) {
	// 적 총알 그리기
		// 왼쪽 아래
	switch (i->dir) {
	case 0:
		// 직진
		i->position.x -= 3;
		break;
	case 1:
		// 왼쪽 위로
		i->position.x -= 3;
		i->position.y += 3;
		break;
	case 2:
		// 왼쪽 아래
		i->position.x -= 3;
		i->position.y -= 3;
		break;
	}
}