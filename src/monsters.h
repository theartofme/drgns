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
#define MONSTER_COUNTER_RESET 40
#define MONSTER_COUNTER_RESET_VARIANCE 25

int monsterSpawnCounter = MONSTER_COUNTER_RESET;

#define MONSTER_FLAG_ALIVE 1
#define MONSTER_FLAG_FLASH 2

// bat stuff

// snake stuff
const int snakeTimerSpeedIncrease = 4;

enum MonsterType {
	BAT,
	SNAKE,
	NUM_MONSTER_TYPES
};

struct ActiveMonster {
	int16_t x;
	int16_t y;
	int16_t vx;
	int16_t vy;
	int16_t timer;
	uint8_t flags;
	uint8_t hp;
	uint8_t flashCountdown;
	MonsterType type;
	Image* image;
};
void initMonster(ActiveMonster* monster, MonsterType type, int x, int y) {
	monster->vx = 0;
	monster->vy = 0;
	monster->timer = 0;
	monster->flags = 0;
	monster->flashCountdown = 0;
	monster->type = type;
	switch (type) {
		case BAT: 
			monster->image = &batImage;
			monster->hp = 30;
			monster->vy = ONE_PIXEL * 4;
			if (x >= PLAY_AREA_W / 2) {
				monster->vx = ONE_PIXEL * -2;
			} else {
				monster->vx = ONE_PIXEL * 2;
			}
			break;
		case SNAKE: 
			monster->image = &snakeImage;
			monster->hp = 15;
			monster->vy = ONE_PIXEL * 1;
			break;
	}
	// Adjust starting x and y based on image dimensions - move x to middle of
	// sprite, and y to bottom of sprite
	monster->x = x - monster->image->width() * ONE_PIXEL / 2;
	monster->y = y - monster->image->height() * ONE_PIXEL;
}

ActiveMonster monsters[MAX_ACTIVE_MONSTERS];

void updateBat(ActiveMonster * bat);
void updateSnake(ActiveMonster * snake);

void updateMonsters() {
	bool spawn = false;
	--monsterSpawnCounter;
	if (monsterSpawnCounter <= 0) {
		monsterSpawnCounter = MONSTER_COUNTER_RESET + random(MONSTER_COUNTER_RESET_VARIANCE);
		spawn = true;
	}
	for (int i = 0; i < MAX_ACTIVE_MONSTERS; ++i) {
		if (monsters[i].flags & MONSTER_FLAG_ALIVE) {
			++(monsters[i].timer);
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
			int x2 = x1 + (monsters[i].image->width() << SUBPIXEL_BITS) - 1;
			int y2 = y1 + (monsters[i].image->height() << SUBPIXEL_BITS) - 1;

			if (collidesWithFlame(x1, y1, x2, y2)) {
				monsters[i].hp -= 1;
				monsters[i].flashCountdown = MONSTER_FLASH_FRAMES;
				monsters[i].flags |= MONSTER_FLAG_FLASH;
				if (monsters[i].hp == 0) {
					monsters[i].flags &= ~MONSTER_FLAG_ALIVE;
					gb.sound.playCancel();
				}
			}

			switch (monsters[i].type) {
				case BAT: updateBat(monsters + i); break;
				case SNAKE: updateSnake(monsters + i); break;
			}

			if (monsters[i].y > PLAY_AREA_H) {
				monsters[i].flags &= ~MONSTER_FLAG_ALIVE;
			}
		} else if (spawn) {
			spawn = false;
			MonsterType type = BAT;
			type = (MonsterType) random(NUM_MONSTER_TYPES);
			initMonster(monsters + i, type, PLAY_AREA_W / 4 + random(PLAY_AREA_W / 2), 0);
			monsters[0].flags = MONSTER_FLAG_ALIVE;
		}
	}
}

void updateBat(ActiveMonster * bat) {
	// move in a bouncy
	bat->vy -= ONE_PIXEL / 4;
	if (bat->vy <= ONE_PIXEL * -2) {
		bat->vy = ONE_PIXEL * 2;
		bat->vx = -bat->vx;
	}
	bat->x += bat->vx;
	bat->y += bat->vy;
}

void updateSnake(ActiveMonster * snake) {
	// move straight down, accelerating
	if (snake->timer >= snakeTimerSpeedIncrease) {
		snake->timer = 0;
		snake->vy += ONE_PIXEL / 2;
	}

	snake->x += snake->vx;
	snake->y += snake->vy;
	if (snake->y > PLAY_AREA_H) {
		snake->flags &= ~MONSTER_FLAG_ALIVE;
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
			int x = monsters[i].x >> SUBPIXEL_BITS;
			int y = monsters[i].y >> SUBPIXEL_BITS;
			int spriteWidth = monsters[i].image->width();
			int spriteHeight = monsters[i].image->height();
			if (x < 0 || y < 0 || x + spriteWidth >= SCREEN_W || y + spriteWidth >= SCREEN_H) {
				// clipping when it's half-on the screen, gamebuino doesn't do this by default
				// TODO: actually I think it does for x/y greater than the screen size, but not for negative x/y?
				// TODO: or maybe that was just because I was using an unsigned int! Just test again
				int xw = SCREEN_W - x;
				if (xw > spriteWidth) xw = spriteWidth;
				if (xw <= 0) continue;

				int yw = SCREEN_H - y;
				if (yw > spriteHeight) yw = spriteHeight;
				if (yw <= 0) continue;

				int xs = -x;
				if (xs < 0) xs = 0; else x = 0;
				if (xs >= spriteWidth) continue;

				int ys = -y;
				if (ys < 0) ys = 0; else y = 0;
				if (ys >= spriteHeight) continue;

				gb.display.drawImage(x, y, *monsters[i].image, xs, ys, xw, yw);
			} else {
				gb.display.drawImage(x, y, *monsters[i].image);
			}
		}
	}
}

