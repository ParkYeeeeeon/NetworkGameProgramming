#include "Utill.h"



void key_input(int key) {

	// Ű �Է� ó��
	// send()�ϸ� �ǰ���??

}

void init_ui(UI& ui) {
	ui.hp_ui_img.Load("Image\\UI\\Life.png");
	ui.bomb_ui_img.Load("Image\\UI\\ITEM_BOMB.png");

	for (int i = 0; i < 2; ++i) {
		ui.hp[i] = 3;
		ui.hp_position[i].x = 25;
		ui.hp_position[i].y = 25 + (i * 550);

		ui.bomb[i] = 3;
		ui.bomb_position[i].x = 31;
		ui.bomb_position[i].y = 70 + (i * 550);
	}

	for (int i = 0; i < 3; ++i) {

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

void draw_map() {
	// map �׸���
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