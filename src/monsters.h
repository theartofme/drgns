#pragma once

#include <Gamebuino-Meta.h>
#include "../img/bat.h"
#include "../img/snake.h"
#include "../img/palette.h"
#include "common.h"
#include "collide.h"
#include "dragon.h"

Image batImage(BAT_IMAGE_DATA);
Image snakeImage(SNAKE_IMAGE_DATA);

#define MAX_ACTIVE_MONSTERS 8
#define MONSTER_FLASH_FRAMES 19

#define MONSTER_FLAG_ALIVE 1
#define MONSTER_FLAG_FLASH 2

enum MonsterType {
	BAT,
	SNAKE
};

struct ActiveMonster {
	uint8_t x;
	uint8_t y;
	uint8_t flags;
	uint8_t hp;
	uint8_t flashCountdown;
	MonsterType type;
	Image* image;
};

ActiveMonster monsters[MAX_ACTIVE_MONSTERS];

void updateMonsters() {
	for (int i = 0; i < MAX_ACTIVE_MONSTERS; ++i) {
		if (monsters[i].flags & MONSTER_FLAG_ALIVE) {
			if (monsters[i].flashCountdown > 0) {
				--monsters[i].flashCountdown;
				if (monsters[i].flashCountdown & 1) {
					monsters[i].flags |= MONSTER_FLAG_FLASH;
				} else {
					monsters[i].flags &= ~MONSTER_FLAG_FLASH;
				}
			}

			int x1 = monsters[i].x;
			int y1 = monsters[i].y;
			int x2 = x1 + monsters[i].image->width() - 1;
			int y2 = y1 + monsters[i].image->height() - 1;

			if (collidesWithFlame(x1, y1, x2, y2)) {
				monsters[i].hp -= 1;
				monsters[i].flashCountdown = MONSTER_FLASH_FRAMES;
				monsters[i].flags |= MONSTER_FLAG_FLASH;
				if (monsters[i].hp == 0) {
					monsters[i].flags &= ~MONSTER_FLAG_ALIVE;
					gb.sound.playCancel();
				}
			}
		}
	}
}

void drawMonsters() {
	for (int i = 0; i < MAX_ACTIVE_MONSTERS; ++i) {
		if (monsters[i].flags & MONSTER_FLAG_ALIVE) {
			// TODO: pull hits out of the loop
			if (monsters[i].flags & MONSTER_FLAG_FLASH) {
				gb.display.setPalette(HIT_PALETTE);
			} else {
				gb.display.setPalette(MONSTER_PALETTE);
			}
			// TODO: clipping when it's half-on the screen
			gb.display.drawImage(monsters[i].x, monsters[i].y, *monsters[i].image);
		}
	}
}

