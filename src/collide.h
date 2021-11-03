#pragma once

bool testBox(int ax1, int ay1, int ax2, int ay2, int bx1, int by1, int bx2, int by2) {
	return (
		ax1 <= bx2 && bx1 <= ax2 &&
		ay1 <= by2 && by1 <= ay2
	);
}