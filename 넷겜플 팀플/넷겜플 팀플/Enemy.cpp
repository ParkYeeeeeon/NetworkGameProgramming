#include "Enemy.h"

void draw_enemy() {
	// 적 그리기
	Enemy mon1[100];
	Enemy mon2[100];
	Enemy mon3[100];

	for (int i = 0; i < 100; i++) // 몬스터 위치 난수 생성
	{
		mon1[i].position.x=(rand() % (display_end_x - display_start_x) + display_start_x)-300;
		mon2[i].position.x = (rand() % (display_end_x - display_start_x) + display_start_x) - 300;
		mon3[i].position.x = (rand() % (display_end_x - display_start_x) + display_start_x) - 300;

		mon1[i].position.y = rand() % (display_end_y - display_start_y) + display_start_y;
		mon2[i].position.y = rand() % (display_end_y - display_start_y) + display_start_y;
		mon3[i].position.y = rand() % (display_end_y - display_start_y) + display_start_y;
	}

	for (int i = 0; i < 100; i++)
	{
		Monster_Draw(mem0dc, mon1[i].position.x, mon1[i].position.y, mon1[i].level, mon1[i].hp);
	}
}



void draw_enemybullet() {
	// 적 총알 그리기
}