#include "Utill.h"

void key_input(int key) {
	// Ű �Է� ó��
	// send()�ϸ� �ǰ���??
	switch (key) {
	case VK_UP:
		break;
	case VK_DOWN:
		break;
	case VK_LEFT:
		break;
	case VK_RIGHT:
		break;
	}
}

void init_ui(UI& ui) {
	ui.hp_ui_img.Load("Image\\UI\\Life.png");
	ui.bomb_ui_img.Load("Image\\UI\\ITEM_BOMB.png");

	for (int i = 0; i < 2; ++i) {
		ui.hp[i] = 10;
		ui.hp_position[i].x = 25;
		ui.hp_position[i].y = 25 + (i * 550);

		ui.bomb[i] = 3;
		ui.bomb_position[i].x = 31;
		ui.bomb_position[i].y = 70 + (i * 550);
	}

	ui.time_position.x = 540;
	ui.time_position.y = 50;
}

void draw_ui(HDC hdc, UI& ui) {
	// ui �׸���
	SetTextColor(hdc, RGB(0, 0, 255));
	SetBkMode(hdc, TRANSPARENT);
	for (int i = 0; i < 2; ++i) {
		for (int j = 0; j < ui.hp[i]; ++j) {
			ui.hp_ui_img.Draw(hdc, ui.hp_position[i].x + (30 * j), ui.hp_position[i].y, 40, 40);
		}
		for (int j = 0; j < ui.bomb[i]; ++j) {
			ui.bomb_ui_img.Draw(hdc, ui.bomb_position[i].x + (30 * j), ui.bomb_position[i].y, 25, 25);
		}
	}
}

void draw_map(HDC hdc, CImage& mapimg) {
	// map �׸���
	SetTextColor(hdc, RGB(0, 0, 255));
	SetBkMode(hdc, TRANSPARENT);
	mapimg.Draw(hdc, 0, 0, 1080, 720);

}

void Connect() {
	// connect �ϱ�
}

void Recv_Packet() {
	// ��Ŷ recv �ϱ�
}

void Check_Packet() {
	// ��Ŷ �����ϱ�
}

void Send_Packet() {
	// ��Ŷ send �ϱ�
}

int get_distance(Location l1, Location l2) {
	return (int)sqrt(pow((l2.x - l1.x), 2) + pow((l2.y - l1.y), 2));
}