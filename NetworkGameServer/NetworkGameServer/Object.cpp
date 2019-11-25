#include "main.h"
#include "Object.h"

void set_player(Player player[2]) {

	player[0].position.x = 0;
	player[0].position.y = 100;

	player[1].position.x = 0;
	player[1].position.y = 400;
}

void add_player_bullet(Player player[2]) {
	for (int i = 0; i < 2; ++i) {
		if (player[i].fire == true) {
			Bullet tmp;
			tmp.position.x = player[i].position.x + 55;
			tmp.position.y = player[i].position.y + 20;
			tmp.type = 5;
			tmp.dir = 0;
			tmp.bullet_type = 0;
			player[i].bullet.emplace_back(tmp);
		}
	}
}

void add_bullet_position(Player player[2]) {
	for (int i = 0; i < 2; ++i) {
		if (player[i].bullet.size() > 0)
			for (std::vector<Bullet>::iterator j = player[i].bullet.begin(); j < player[i].bullet.end(); ++j) {
				j->position.x += 10;
			}
	}
}