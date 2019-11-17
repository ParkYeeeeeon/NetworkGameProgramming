#include "Enemy.h"

CImage Monster_image[10];
CImage Monster_bullet[3]; // 몬스터 총알
Enemy mon1[MONSTER_COUNT];
Enemy mon2[MONSTER_COUNT];
Enemy mon3[MONSTER_COUNT];
vector<Bullet> bullet;

void init_Monster_Image() {
	Monster_image[0].Load("Image\\Monster\\미니몬스터1.png");
	Monster_image[1].Load("Image\\Monster\\미니몬스터2.png");
	Monster_image[2].Load("Image\\Monster\\미니몬스터3.png");
	srand((unsigned int)time(NULL));

	for (int i = 0; i < MONSTER_COUNT; i++) // 몬스터 위치 난수 생성
	{
		mon1[i].position.x = (rand() % (display_end_x - display_start_x) + display_start_x) - 300;
		mon1[i].position.y = rand() % (display_end_y - display_start_y) + display_start_y;
		mon2[i].position.x = (rand() % (display_end_x - display_start_x) + display_start_x) - 300;
		mon3[i].position.x = (rand() % (display_end_x - display_start_x) + display_start_x) - 300;
		mon2[i].position.y = rand() % (display_end_y - display_start_y) + display_start_y;
		mon3[i].position.y = rand() % (display_end_y - display_start_y) + display_start_y;

	}
}

void revival_enemy() {
	// 몬스터가 죽었을 경우 랜덤한 위치에 다시 생성 시킨다.
	for (int i = 0; i < MONSTER_COUNT; i++)
	{
		if (mon1[i].alive == false) {
			mon1[i].position.x = (rand() % (display_end_x - display_start_x) + display_start_x) - 300;
			mon1[i].position.y = rand() % (display_end_y - display_start_y) + display_start_y;
			mon1[i].alive = true;
		}

		if (mon2[i].alive == false) {
			mon2[i].position.x = (rand() % (display_end_x - display_start_x) + display_start_x) - 300;
			mon2[i].position.y = rand() % (display_end_y - display_start_y) + display_start_y;
			mon2[i].alive = true;
		}

		if (mon3[i].alive == false) {
			mon3[i].position.x = (rand() % (display_end_x - display_start_x) + display_start_x) - 300;
			mon3[i].position.y = rand() % (display_end_y - display_start_y) + display_start_y;
			mon3[i].alive = true;
		}
	}
}

void add_enemy_bullet(int monsterKind, int monster_id) {
	switch (monsterKind)
	{
	case 0:
	{
		if (mon1[monster_id].alive == true) {
			Bullet tmp_bullet;
			tmp_bullet.position.x = mon1[monster_id].position.x - 20;
			tmp_bullet.position.y = mon1[monster_id].position.y + 20;
			tmp_bullet.type = 0;
			tmp_bullet.dir = 0;
			tmp_bullet.bullet_type = 0;
			bullet.push_back(tmp_bullet);
		}
	}
	break;
	case 1:
	{
		if (mon2[monster_id].alive == true) {
			Bullet tmp_bullet;
			tmp_bullet.position.x = mon2[monster_id].position.x - 20;
			tmp_bullet.position.y = mon2[monster_id].position.y;
			tmp_bullet.type = 0;
			tmp_bullet.dir = 0;
			tmp_bullet.bullet_type = 1;
			bullet.push_back(tmp_bullet);

			tmp_bullet.position.x = mon2[monster_id].position.x - 20;
			tmp_bullet.position.y = mon2[monster_id].position.y + 40;
			tmp_bullet.type = 0;
			tmp_bullet.dir = 0;
			tmp_bullet.bullet_type = 1;
			bullet.push_back(tmp_bullet);

		}
	}
	break;
	case 2:
	{
		// mon3이 살아 있을 경우에만 총알을 그린다.
		if (mon3[monster_id].alive == true) {
			for (int i = 0; i < 3; ++i) {
				Bullet tmp_bullet;
				tmp_bullet.position.x = mon3[monster_id].position.x - 20;
				tmp_bullet.position.y = mon3[monster_id].position.y + 20;
				tmp_bullet.type = 0;
				tmp_bullet.bullet_type = 0;
				tmp_bullet.dir = i;	// 직진 // 왼쪽 위로 // 왼쪽 아래
				bullet.push_back(tmp_bullet);
			}
		}
	}
	break;
	}
}

void init_Monster_Bullet_Image()
{
	Monster_bullet[0].Load("Image\\Monster\\적총알기본.png");
	// 총알 vector를 미리 200개 할당 해놓는다.
	// 만약 200개 이상일 경우 숫자를 더 늘려줘야지, 나중에 문제가 생기지 않는다.
	bullet.reserve(100000);
}

void draw_enemybullet(HDC hdc) {
	for (vector<Bullet>::iterator i = bullet.begin(); i < bullet.end();)
	{
		// 총알이 영역 밖으로 나갔을 경우 삭제
		if ((i->position.x <= 0 || i->position.y <= 0 || i->position.y >= 720)) {
			// 범위를 벗어나면 삭제를 해준다.
			i = bullet.erase(i);
		}
		else {
			change_enemy_bullet(i);
			Bullet_Draw(hdc, i->position.x, i->position.y, i->type, i->bullet_type);
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
	switch (monsterKind)
	{
	case 0:
		// mon1
		mon1[monster_id].ani += 1;
		if (mon1[monster_id].ani >= 4) {
			mon1[monster_id].ani = 0;
		}
		break;
	case 1:
		// mon2
		mon2[monster_id].ani += 1;
		if (mon2[monster_id].ani >= 4) {
			mon2[monster_id].ani = 0;
		}
		break;
	case 2:
		// mon3
		mon3[monster_id].ani += 1;
		if (mon3[monster_id].ani >= 4) {
			mon3[monster_id].ani = 0;
		}
		break;
	}

}

void Monster_Draw(HDC hdc, int x, int y, int Kind, int ani, int hp) {
	SetTextColor(hdc, RGB(0, 0, 255));
	SetBkMode(hdc, TRANSPARENT);
	int monsterX = 20, monsterY = 16;
	Monster_image[Kind].Draw(hdc, 0 + (x * 1), 0 + (y * 1), 40, 40, 0 + (ani * 20), 0, monsterX, monsterY);
}

void draw_enemy(HDC hdc) {
	// Monster1 적 그리기
	for (int i = 0; i < MONSTER_COUNT; i++)
	{
		if (mon1[i].alive == true) {
			Monster_Draw(hdc, mon1[i].position.x, mon1[i].position.y, 0, rand() % 5, 100);
		}
		if (mon2[i].alive == true) {
			Monster_Draw(hdc, mon2[i].position.x, mon2[i].position.y, 1, rand() % 5, 100);
		}
		if (mon3[i].alive == true) {
			Monster_Draw(hdc, mon3[i].position.x, mon3[i].position.y, 2, rand() % 5, 100);
		}
	}
}

void change_enemy_location(int monsterKind, int monster_id)
{
	switch (monsterKind)
	{
	case 0:
		// mon1 이 살아 있을 경우에만 움직임이 작동한다.
		if (mon1[monster_id].alive == true) {
			mon1[monster_id].position.x = mon1[monster_id].position.x - 2;
			if (mon1[monster_id].position.x <= 0 || mon1[monster_id].position.y <= 0 || mon1[monster_id].position.y >= 720) {
				// 범위를 벗어나면 죽었다고 처리를 한다.
				mon1[monster_id].alive = false;
			}
		}
		break;
	case 1:
		// mon2 이 살아 있을 경우에만 움직임이 작동한다.
		if (mon2[monster_id].alive == true) {
			mon2[monster_id].position.x = mon2[monster_id].position.x - 3;
			if (mon2[monster_id].position.x <= 0 || mon2[monster_id].position.y <= 0 || mon2[monster_id].position.y >= 720) {
				// 범위를 벗어나면 죽었다고 처리를 한다.
				mon2[monster_id].alive = false;
			}
		}
		break;
	case 2:
		// mon3 이 살아 있을 경우에만 움직임이 작동한다.
		if (mon3[monster_id].alive == true) {
			mon3[monster_id].position.x = mon3[monster_id].position.x - 5;
			if (mon3[monster_id].position.x <= 0 || mon3[monster_id].position.y <= 0 || mon3[monster_id].position.y >= 720) {
				// 범위를 벗어나면 죽었다고 처리를 한다.
				mon3[monster_id].alive = false;
			}
		}
		break;


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

void change_enemy_bullet(vector<Bullet>::iterator i) {
	// 적 총알 그리기
		// 왼쪽 아래
	switch (i->dir) {
	case 0:
		// 직진
		i->position.x -= 1;
		break;
	case 1:
		// 왼쪽 위로
		i->position.x -= 1;
		i->position.y += 1;
		break;
	case 2:
		// 왼쪽 아래
		i->position.x -= 1;
		i->position.y -= 1;
		break;
	}
}