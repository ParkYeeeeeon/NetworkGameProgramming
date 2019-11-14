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
	// 몬스터 Bullet 을 넣기 위하여 몬스터 만큼 돌린다.
	for (int i = 0; i < MONSTER_COUNT; i++)
	{
		// Bullet 구조체를 먼저 선언하여 해당 몬스터의 좌표와, 타입을 넣어준다.
		Bullet bullet;
		bullet.position.x = mon1[i].position.x;
		bullet.position.y = mon1[i].position.y;
		mon1[i].bullet.reserve(25); // 벡터의 메모리크기 미리 할당
		bullet.type = 0;
		// j 가 도는 만큼 해당 몬스터의 bullet에 추가 된다.
		for (int j = 0; j < 1; ++j) {
			mon1[i].bullet.push_back(bullet);
		}
	}

}

void draw_enemybullet(HDC hdc) {
	// 몬스터 Bullet을 그리기 위하여 몬스터 만큼 돌린다.
	for (int i = 0; i < MONSTER_COUNT; i++)
	{
		// mon1의 Vector bullet구조체에 들어있는 만큼 for문을 돌린다.
		for (auto bullet : mon1[i].bullet) {
			// 해당 Vector에 들어있는 bullet을 가져와서 위치에 그려준다.
			// ex) 위에서 10개의 Bullet를 그렸으니, 몬스터가 처음 지정된 위치에 bullet가 10개가 그대로 그려져 있는다.
			Bullet_Draw(hdc, bullet.position.x, bullet.position.y, bullet.type);
		}

	}
}

void Bullet_Draw(HDC hdc, int x, int y, int Kind) {
	SetTextColor(hdc, RGB(0, 0, 255));
	SetBkMode(hdc, TRANSPARENT);
	int bulletX = 50, bulletY = 50;
	Monster_bullet[Kind].Draw(hdc, 0 + (x * 1), 0 + (y * 1), 10, 10, 0, 0, bulletX, bulletY);
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
		mon1[monster_id].position.x = mon1[monster_id].position.x - 2;
		break;
	case 1:
		// mon2
		mon2[monster_id].position.x = mon2[monster_id].position.x - 3;
		break;
	case 2:
		// mon3
		mon3[monster_id].position.x = mon3[monster_id].position.x - 5;
		break;


	}
}

void change_enemy_bullet(int monsterKind,int monster_id) {
	// 적 총알 그리기
	switch (monsterKind)
	{
	case 0:
		// mon1
		for (vector<Bullet>::iterator i = mon1[monster_id].bullet.begin(); i < mon1[monster_id].bullet.end(); i++)
		{
			i->position.x -= 1;
		}
	}
}