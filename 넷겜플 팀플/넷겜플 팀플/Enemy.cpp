#include "Enemy.h"

CImage Monster_image[10];
CImage Monster_bullet[3]; // 몬스터 총알
Enemy mon1[MONSTER_COUNT];
Enemy mon2[MONSTER_COUNT];
Enemy mon3[MONSTER_COUNT];

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

void init_Monster_Bullet_Image()
{
	Monster_bullet[0].Load("Image\\Monster\\적총알기본.png");
	for (int i = 0; i < MONSTER_COUNT; i++) // 몬스터 위치 난수 생성
	{
		//mon1[i].bullet

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
		Monster_Draw(hdc, mon1[i].position.x, mon1[i].position.y, 0, rand() % 5, 100);
		Monster_Draw(hdc, mon2[i].position.x, mon2[i].position.y, 1, rand() % 5, 100);
		Monster_Draw(hdc, mon3[i].position.x, mon3[i].position.y, 2, rand() % 5, 100);
	}
}

void change_enemy_location(int monsterKind, int monster_id)
{
	switch (monsterKind)
	{
	case 0:
		// mon1
		mon1[monster_id].position.x = mon1[monster_id].position.x - 1;
		break;
	case 1:
		// mon2
		mon2[monster_id].position.x = mon2[monster_id].position.x - 3;
		break;
	case 2:
		// mon3
		mon3[monster_id].position.x = mon3[monster_id].position.x - 7;
		break;


	}
}

void draw_enemybullet() {
	// 적 총알 그리기
}