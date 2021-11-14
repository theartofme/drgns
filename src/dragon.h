#pragma once

#include <Gamebuino-Meta.h>
#include "../img/dragon.h"
#include "../img/flame.h"
#include "../img/palette.h"
#include "common.h"
#include "lights.h"
#include "collide.h"

void updatePlayer();
void updateFlames();
void flameBox(int* x1, int* y1, int* x2, int* y2);

Image dragonImage(DRAGON_IMAGE_DATA);
#define PLAYER_SPEED_X (2 << SUBPIXEL_BITS)
#define PLAYER_SPEED_Y (1 << SUBPIXEL_BITS)
#define PLAYER_W (dragonImage.width() << SUBPIXEL_BITS)
#define PLAYER_H (dragonImage.height() << SUBPIXEL_BITS)

struct Player {
	int16_t x;
	int16_t y;
};
Player player;

Image flameImage(FLAME_BREATH_IMAGE_DATA);
#define FLAME_DETACHED_DURATION ((FLAME_BREATH_IMAGE_DATA[2] - FLAME_ATTACH_FRAME_COUNT) * FLAME_BREATH_IMAGE_DATA[3])
// first four frames always attach to the dragon
#define FLAME_ATTACH_FRAME_COUNT 4
// last two frames are short too, but at the other end of the image
#define SHORT_END_FLAME (2 * FLAME_BREATH_IMAGE_DATA[3])
#define BREATH_W (flameImage.width() << SUBPIXEL_BITS)
#define BREATH_H (flameImage.height() << SUBPIXEL_BITS)
struct Flames {
	uint8_t durationRemaining;
	int16_t x;
	int16_t y;
};
Flames flames;

void drawPlayer() {
	gb.display.setPalette(DRAGON_PALETTE_RED);
	gb.display.drawImage(player.x >> SUBPIXEL_BITS, player.y >> SUBPIXEL_BITS, dragonImage);
	if (flames.durationRemaining > 0) {
		gb.display.drawImage(flames.x >> SUBPIXEL_BITS, flames.y >> SUBPIXEL_BITS, flameImage);
	}
}

void updatePlayer() {
	if (gb.buttons.repeat(BUTTON_LEFT, 0)) {
		player.x -= PLAYER_SPEED_X;
		if (player.x < 0) {
			player.x = 0;
		}
	}
	if (gb.buttons.repeat(BUTTON_RIGHT, 0)) {
		player.x += PLAYER_SPEED_X;
		if (player.x >= PLAY_AREA_W - PLAYER_W) {
			player.x = PLAY_AREA_W - PLAYER_W - 1;
		}
	}
	if (gb.buttons.repeat(BUTTON_UP, 0)) {
		player.y -= PLAYER_SPEED_Y;
		if (player.y < BREATH_H) {
			player.y = BREATH_H;
		}
	}
	if (gb.buttons.repeat(BUTTON_DOWN, 0)) {
		player.y += PLAYER_SPEED_Y;
		if (player.y >= PLAY_AREA_H - PLAYER_H) {
			player.y = PLAY_AREA_H - PLAYER_H - 1;
		}
	}

	updateFlames();
}

#define MAX_FLAME_STRENGTH 20
#define MAX_FLAME_ENDURANCE 15
#define MAX_FLAME_COOLDOWN 10
#define FLAME_SPEED 2
#define FLAME_RECOVERY 1

uint8_t flameStrength = 0;
uint8_t flameCooldown = 0;
uint8_t flameEndurance = MAX_FLAME_ENDURANCE;
bool isFlaming = false;

void updateFlames() {
	// TODO: more than one flame active - 2-3 should be enough, for when a new
	// flame is started but the old one isn't attached and hasn't finished its
	// animation.
	/*
	var active_flame = 0
	var flames[2]
	*/

	if (flameCooldown > 0) {
		--flameCooldown;
	}

	if (gb.buttons.repeat(BUTTON_A, 0) && flameEndurance > 0 && (isFlaming || flameCooldown == 0)) {
		if (!isFlaming) {
			gb.sound.play("flame.wav");
			// active_flame = active_flame ^ 1 // swap active flame
		} else {
			// TODO: continuous flame sound
		}
		isFlaming = true;
		flameCooldown = MAX_FLAME_COOLDOWN;
		flameStrength += FLAME_SPEED;
		if (flameStrength > MAX_FLAME_STRENGTH) {
			flameStrength = MAX_FLAME_STRENGTH;
		}
		ledBreathStrength = (255 * flameStrength) / MAX_FLAME_STRENGTH;
		--flameEndurance;
		flameImage.setFrame((FLAME_ATTACH_FRAME_COUNT * flameStrength) / (MAX_FLAME_STRENGTH + 1));
		flames.durationRemaining = FLAME_DETACHED_DURATION;
		flames.x = player.x;
		flames.y = player.y - BREATH_H;
	}
	if ((!gb.buttons.repeat(BUTTON_A, 0) || flameEndurance == 0) && isFlaming) {
		isFlaming = false;
		flameStrength = 0;
		flameImage.setFrame(FLAME_ATTACH_FRAME_COUNT);
		flames.durationRemaining = FLAME_DETACHED_DURATION;
	} else if (!gb.buttons.repeat(BUTTON_A, 0) && !isFlaming) {
		if (flameEndurance < MAX_FLAME_ENDURANCE) {
			flameEndurance += FLAME_RECOVERY;
			if (flameEndurance > MAX_FLAME_ENDURANCE) {
				flameEndurance = MAX_FLAME_ENDURANCE;
			}
			ledBreathStrength = 0;
			ledBreathRecharge = 255 - (flameEndurance * 255) / MAX_FLAME_ENDURANCE;
		}
	}
	if (flames.durationRemaining > 0) {
		--flames.durationRemaining;
	}
}

bool collidesWithFlame(int x1, int y1, int x2, int y2) {
	if (flames.durationRemaining > 0) {
		int fx1, fy1, fx2, fy2;
		flameBox(&fx1, &fy1, &fx2, &fy2);
		return testBox(x1, y1, x2, y2, fx1, fy1, fx2, fy2);
	} else {
		return false;
	}
}

void flameBox(int* x1, int* y1, int* x2, int* y2) {
	if (flames.durationRemaining > 0) {
		*x1 = flames.x;
		*y1 = flames.y;
		*x2 = flames.x + BREATH_W;
		*y2 = flames.y + BREATH_H;
		if (flames.durationRemaining < SHORT_END_FLAME) {
			*y2 = flames.y + BREATH_H / 2;
		}
	} else {
		*x1 = *y1 = *x2 = *y2 = 0;
	}
}