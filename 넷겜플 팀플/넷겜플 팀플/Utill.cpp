#include "Utill.h"

void key_input(int key) {
	// 키 입력 처리
	// send()하면 되겠지??
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
	// ui 그리기
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
	// map 그리기
	SetTextColor(hdc, RGB(0, 0, 255));
	SetBkMode(hdc, TRANSPARENT);
	mapimg.Draw(hdc, 0, 0, 1080, 720);

}

void Connect() {
	// connect 하기
}

void Recv_Packet() {
	// 패킷 recv 하기
}

void Check_Packet() {
	// 패킷 구분하기
}

void Send_Packet() {
	// 패킷 send 하기
}

int get_distance(Location l1, Location l2) {
	return (int)sqrt(pow((l2.x - l1.x), 2) + pow((l2.y - l1.y), 2));
}