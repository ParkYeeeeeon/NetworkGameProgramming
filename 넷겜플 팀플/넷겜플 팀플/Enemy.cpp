#include "Enemy.h"

CImage Monster_image[10];
Enemy mon1[100];
Enemy mon2[100];
Enemy mon3[100];

void init_Monster_Image() {
	Monster_image[0].Load("Image\\Monster\\�̴ϸ���1.png");
	Monster_image[1].Load("Image\\Monster\\�̴ϸ���2.png");
	Monster_image[2].Load("Image\\Monster\\�̴ϸ���3.png");
	srand((unsigned int)time(NULL));

	for (int i = 0; i < 100; i++) // ���� ��ġ ���� ����
	{
		mon1[i].position.x = (rand() % (display_end_x - display_start_x) + display_start_x) - 300;
		mon1[i].position.y = rand() % (display_end_y - display_start_y) + display_start_y;
		mon2[i].position.x = (rand() % (display_end_x - display_start_x) + display_start_x) - 300;
		mon3[i].position.x = (rand() % (display_end_x - display_start_x) + display_start_x) - 300;
		mon2[i].position.y = rand() % (display_end_y - display_start_y) + display_start_y;
		mon3[i].position.y = rand() % (display_end_y - display_start_y) + display_start_y;
		
	}
}

void Monster_Draw(HDC hdc, int x, int y, int Kind, int ani, int hp) {
	SetTextColor(hdc, RGB(0, 0, 255));
	SetBkMode(hdc, TRANSPARENT);
	int monsterX = 20, monsterY = 16;
	Monster_image[Kind].Draw(hdc, 0 + (x * 1), 0 + (y * 1), 40, 40, 0 + (ani * 20), 0, monsterX, monsterY);
}

void draw_enemy(HDC hdc) {
	// Monster1 �� �׸���
	for (int i = 0; i < 100; i++)
	{
		Monster_Draw(hdc, mon1[i].position.x, mon1[i].position.y, 0, rand() % 5, 100);
		Monster_Draw(hdc, mon2[i].position.x, mon2[i].position.y, 1, rand() % 5, 100);
		Monster_Draw(hdc, mon3[i].position.x, mon3[i].position.y, 2, rand() % 5, 100);
	}
}



void draw_enemybullet() {
	// �� �Ѿ� �׸���
}