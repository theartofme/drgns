#pragma once

#include <Gamebuino-Meta.h>

const Color DRAGON_PALETTE_RED[] = {

    (Color) 0x0000, // 0x0
    (Color) 0x90e8, // 0x1
    (Color) 0xab05, // 0x2
    (Color) 0xac82, // 0x3
    (Color) 0x9471, // 0x4
    (Color) 0x4c73, // 0x5
    (Color) 0x5393, // 0x6
    (Color) 0x5211, // 0x7
    (Color) 0x1b6c, // 0x8
    (Color) 0x7469, // 0x9
    (Color) 0xa4e8, // 0xa
    (Color) 0x9aaa, // 0xb
    (Color) 0x9b6f, // 0xc
    (Color) 0x4c6b, // 0xd
    (Color) 0x4b6a, // 0xe
    (Color) 0xf81f  // 0xf

};



#define MONSTER_PALETTE DRAGON_PALETTE_RED
const Color HIT_PALETTE[16] = {
	WHITE,
	WHITE,
	WHITE,
	WHITE,
	WHITE,
	WHITE,
	WHITE,
	WHITE,
	WHITE,
	WHITE,
	WHITE,
	WHITE,
	WHITE,
	WHITE,
	WHITE,
    (Color) 0xf81f  // 0xf
};
