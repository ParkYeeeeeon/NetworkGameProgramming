#include "Enemy.h"

void draw_enemy() {
	// 적 그리기
	vector<Location> mon1(100);
	vector<Location> mon2(100);
	vector<Location> mon3(100);

	for (int i = 0; i < 100; i++) // 몬스터 위치 난수 생성
	{
		mon1[i].x=(rand() % (display_end_x - display_start_x) + display_start_x)+300;
		mon2[i].x = (rand() % (display_end_x - display_start_x) + display_start_x)+300;
		mon3[i].x = (rand() % (display_end_x - display_start_x) + display_start_x)+300;

		mon1[i].y = rand() % (display_end_y - display_start_y) + display_start_y;
		mon2[i].y = rand() % (display_end_y - display_start_y) + display_start_y;
		mon3[i].y = rand() % (display_end_y - display_start_y) + display_start_y;
	}
}

//void Monster_Draw(HDC hdc, int x, int y, int Kind, int hp) {
//	sprintf(PrintData, "%d", hp);
//	SetTextColor(hdc, RGB(0, 0, 255));
//	SetBkMode(hdc, TRANSPARENT);
//	SetTextAlign(hdc, TA_CENTER);
//	TextOut(hdc, 20 + (x * 40), -14 + (y * 40), PrintData, strlen(PrintData));
//	Monster_image[Kind].Draw(hdc, 0 + (x * 40), 0 + (y * 40), 40, 40, 0, 0, Monster_image[Kind].GetWidth(), Monster_image[Kind].GetHeight());
//}

void draw_enemybullet() {
	// 적 총알 그리기
}