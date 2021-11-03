#include <Gamebuino-Meta.h>

#include "src/common.h"
#include "src/lights.h"
#include "src/dragon.h"
#include "src/monsters.h"

void setup() {
	gb.begin();
	player.x = (PLAY_AREA_W - dragonImage.width()) / 2;
	player.y = PLAY_AREA_H - dragonImage.height();
	monsters[0].x = (PLAY_AREA_W - dragonImage.width()) / 2;
	monsters[0].y = 0;
	monsters[0].flags = MONSTER_FLAG_ALIVE;
	monsters[0].type = BAT;
	monsters[0].image = &batImage;
	monsters[0].hp = 50;
}    

void loop() {
	while(!gb.update());

	updatePlayer();
	updateMonsters();
	ledUpdate();

	gb.display.clear(LIGHTBLUE);
	drawMonsters();
	drawPlayer();
}
