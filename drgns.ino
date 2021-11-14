#include <Gamebuino-Meta.h>

#include "src/common.h"
#include "src/lights.h"
#include "src/dragon.h"
#include "src/monsters.h"

void setup() {
	gb.begin();
	player.x = (PLAY_AREA_W - dragonImage.width() * ONE_PIXEL) / 2;
	player.y = PLAY_AREA_H - dragonImage.height() * ONE_PIXEL;
	initMonster(monsters + 0, BAT, 0, 0);
	monsters[0].flags = MONSTER_FLAG_ALIVE;
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
