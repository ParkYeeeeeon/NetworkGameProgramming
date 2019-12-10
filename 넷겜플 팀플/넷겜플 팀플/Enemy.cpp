#include "Enemy.h"

CImage Monster_image[10];
CImage Monster_bullet[3]; // 몬스터 총알
Enemy monster[MONSTER_COUNT];
list<Bullet> bullet;

void init_Monster_Image() {
	Monster_image[0].Load("Image\\Monster\\미니몬스터1.png");
	Monster_image[1].Load("Image\\Monster\\미니몬스터2.png");
	Monster_image[2].Load("Image\\Monster\\미니몬스터3.png");
	srand((unsigned int)time(NULL));
}

void revival_enemy() {
	//// 몬스터가 죽었을 경우 랜덤한 위치에 다시 생성 시킨다.
	//for (int i = 0; i < MONSTER_COUNT; i++)
	//{
	//	if (mon1[i].alive == false) {
	//		mon1[i].position.x = rand()%300 + display_end_x;
	//		mon1[i].position.y = rand() % (display_end_y - display_start_y) + display_start_y;
	//		//if (mon1[i].position.x < display_end_x)
	//		{
	//			mon1[i].alive = true;
	//		}
	//	}

	//	if (mon2[i].alive == false) {
	//		mon2[i].position.x = rand() % 300 + display_end_x;
	//		mon2[i].position.y = rand() % (display_end_y - display_start_y) + display_start_y;
	//		//if (mon2[i].position.x < display_end_x)
	//		{
	//			mon2[i].alive = true;
	//		}
	//	}

	//	if (mon3[i].alive == false) {
	//		mon3[i].position.x = rand() % 300 + display_end_x;
	//		mon3[i].position.y = rand() % (display_end_y - display_start_y) + display_start_y;
	//		//if (mon3[i].position.x < display_end_x)
	//		{
	//			mon3[i].alive = true;
	//		}
	//	}
	//}
}

void add_enemy_bullet() {
	for (int mi = 0; mi < MONSTER_COUNT; mi++) {
		// 몬스터가 살아 있을 경우에만 처리 한다.
		if (monster[mi].alive == false)
			continue;

		switch (monster[mi].kind) {
		case 0:
		{
			Bullet tmp_bullet;
			tmp_bullet.position.x = monster[mi].position.x - 20;
			tmp_bullet.position.y = monster[mi].position.y + 20;
			tmp_bullet.type = 0;
			tmp_bullet.dir = 0;
			tmp_bullet.bullet_type = 0;
			tmp_bullet.dir = 0;
			bullet.push_back(tmp_bullet);
		}
		break;

		case 1:
		{
			Bullet tmp_bullet;
			tmp_bullet.position.x = monster[mi].position.x - 20;
			tmp_bullet.position.y = monster[mi].position.y;
			tmp_bullet.type = 0;
			tmp_bullet.dir = 0;
			tmp_bullet.bullet_type = 1;
			tmp_bullet.dir = 0;
			bullet.push_back(tmp_bullet);

			tmp_bullet.position.x = monster[mi].position.x - 20;
			tmp_bullet.position.y = monster[mi].position.y + 40;
			tmp_bullet.type = 0;
			tmp_bullet.dir = 0;
			tmp_bullet.bullet_type = 1;
			tmp_bullet.dir = 0;
			bullet.push_back(tmp_bullet);
		}
		break;

		case 2:
		{
			for (int j = 0; j < 3; ++j) {
				Bullet tmp_bullet;
				tmp_bullet.position.x = monster[mi].position.x - 20;
				tmp_bullet.position.y = monster[mi].position.y + 20;
				tmp_bullet.type = 0;
				tmp_bullet.dir = 0;
				tmp_bullet.bullet_type = 0;
				tmp_bullet.dir = j;	// 직진 // 왼쪽 위로 // 왼쪽 아래
				bullet.push_back(tmp_bullet);
			}
		}
		break;

		}
	}
}

void init_Monster_Bullet_Image()
{
	Monster_bullet[0].Load("Image\\Monster\\적총알기본.png");

}

void draw_enemybullet(HDC hdc) {
	for (list<Bullet>::iterator i = bullet.begin(); i != bullet.end();)
	{
		if (i->draw == false) {
			i = bullet.erase(i);
			continue;
		}
		// 총알이 영역 밖으로 나갔을 경우 삭제
		if ((i->position.x <= 0 || i->position.y <= 0 || i->position.y >= 720)) {
			// 범위를 벗어나면 삭제를 해준다.
			i = bullet.erase(i);
		}
		else {
			Bullet_Draw(hdc, i->position.x, i->position.y, i->type, i->bullet_type);
			++i;
		}
	}
}

void move_enemybullet() {
	for (list<Bullet>::iterator i = bullet.begin(); i != bullet.end();)
	{
		// 총알이 영역 밖으로 나갔을 경우 삭제
		if ((i->position.x <= 0 || i->position.y <= 0 || i->position.y >= 720) || i->draw == false) {
			// 범위를 벗어나면 삭제를 해준다.
			i = bullet.erase(i);
		}
		else {
			change_enemy_bullet(i);
			++i;
		}
	}
}

void Bullet_Draw(HDC hdc, int x, int y, int Kind, int bullettype) {
	SetTextColor(hdc, RGB(0, 0, 255));
	SetBkMode(hdc, TRANSPARENT);
	int bulletX = 50, bulletY = 50;

	if (bullettype == 1) {
		// 총알을 가로로 길게 만들기
		Monster_bullet[Kind].Draw(hdc, 0 + (x * 1), 0 + (y * 1), 20, 10, 0, 0, bulletX, bulletY);
	}
	else {
		Monster_bullet[Kind].Draw(hdc, 0 + (x * 1), 0 + (y * 1), 10, 10, 0, 0, bulletX, bulletY);
	}
}

void change_enemy_ani(int monsterKind, int monster_id) {
	monster[monster_id].ani += 1;
	if (monster[monster_id].ani >= 4) {
		monster[monster_id].ani = 0;
	}
}

void Monster_Draw(HDC hdc, int x, int y, int Kind, int ani, int hp) {
	SetTextColor(hdc, RGB(0, 0, 255));
	SetBkMode(hdc, TRANSPARENT);
	int monsterX = 20, monsterY = 16;
	Monster_image[Kind].Draw(hdc, 0 + (x * 1), 0 + (y * 1), 40, 40, 0 + (ani * 20), 0, monsterX, monsterY);
}

void draw_enemy(HDC hdc) {
	// Monster 적 그리기
	for (int i = 0; i < MONSTER_COUNT; i++)
	{
		if (monster[i].alive == true) {
			Monster_Draw(hdc, monster[i].position.x, monster[i].position.y, monster[i].kind, rand() % 5, 100);
		}
	}
}

void change_enemy_location(int monsterKind, int monster_id)
{
	// 몬스터 위치를 먼저 이동 시킨다.
	switch (monsterKind) {
	case 0:
		monster[monster_id].position.x = monster[monster_id].position.x - 2;
		break;
	case 1:
		monster[monster_id].position.x = monster[monster_id].position.x - 3;
		break;
	case 2:
		monster[monster_id].position.x = monster[monster_id].position.x - 5;
		break;
	}

	if (monster[monster_id].position.x <= 0 || monster[monster_id].position.y <= 0 || monster[monster_id].position.y >= 720) {
		// 범위를 벗어나면 죽었다고 처리를 한다.
		monster[monster_id].alive = false;
	}
}

void draw_bullet_status(HDC mem0dc) {
	static char isDebugData[500];
	sprintf(isDebugData, "Bullet Count : %d", bullet.size());
	SetTextColor(mem0dc, RGB(255, 0, 0));
	SetBkMode(mem0dc, OPAQUE);
	SetTextAlign(mem0dc, TA_TOP);
	TextOut(mem0dc, 0, 0, isDebugData, strlen(isDebugData));
}

void change_enemy_bullet(list<Bullet>::iterator i) {
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