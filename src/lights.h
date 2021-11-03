#pragma once

#include <Gamebuino-Meta.h>

// All the various things that can affect the LEDs are stored in globals and updated
// in various update methods in the code. The ledUpdate() method here weighs them
// and decides which ones actually get to affect the LED output, and sets the LEDs
// accordingly. It also handles decaying them, so if they're not updated constantly
// the LEDs will fade out.

uint8_t ledBreathStrength = 0;
uint8_t ledBreathRecharge = 0;
uint8_t ledDamage = 0;
uint8_t flickerCounter = 0;

void ledUpdate() {
	++flickerCounter;
	uint8_t flicker = 0;
	if (flickerCounter & 3) flicker = 1;

	if (ledBreathStrength > 0) {
		for (uint8_t i = 0; i < 4; ++i) {
			int intensity = ((ledBreathStrength - i * 64) * 255) / 64;
			if (intensity < 0) intensity = 0;
			if (intensity > 255) intensity = 255;
			if (i == 3) intensity *= flicker;
			Color lightColour = gb.createColor(intensity, ledBreathStrength * intensity / 512, 0);
			gb.lights.drawPixel(0, 3-i, lightColour);
			gb.lights.drawPixel(1, 3-i, lightColour);
		}
	} else if (ledBreathRecharge > 0) {
		for (uint8_t i = 0; i < 4; ++i) {
			int intensity = ((ledBreathRecharge - i * 64) * 255) / 64;
			if (intensity < 0) intensity = 0;
			if (intensity > 255) intensity = 255;
			Color lightColour = gb.createColor(0, 0, intensity);
			gb.lights.drawPixel(0, i, lightColour);
			gb.lights.drawPixel(1, i, lightColour);
		}
	} else {
		gb.lights.clear();
	}
	if (ledBreathStrength > 3) {
		ledBreathStrength -= 4;
	} else {
		ledBreathStrength = 0;
	}
	if (ledBreathRecharge > 3) {
		ledBreathRecharge -= 4;
	} else {
		ledBreathRecharge = 0;
	}
}
