void gdImageAALine (gdImagePtr im, int x1, int y1, int x2, int y2, int col)
{
	/* keep them as 32bits */
	long x, y, inc, frac;
	long dx, dy,tmp;

	/* 2.0.10: Nick Atty: clip to edges of drawing rectangle, return if no points need to be drawn */
	if (!clip_1d(&x1,&y1,&x2,&y2,gdImageSX(im)-1) || !clip_1d(&y1,&x1,&y2,&x2,gdImageSY(im)-1)) {
		return;
	}

	dx = x2 - x1;
	dy = y2 - y1;

	if (dx == 0 && dy == 0) {
		return;
	}
	if (abs(dx) > abs(dy)) {
		if (dx < 0) {
			tmp = x1;
			x1 = x2;
			x2 = tmp;
			tmp = y1;
			y1 = y2;
			y2 = tmp;
			dx = x2 - x1;
			dy = y2 - y1;
		}
		y = y1;
		inc = (dy * 65536) / dx;
		frac = 0;
		for (x = x1; x <= x2; x++) {
			gdImageSetAAPixelColor(im, x, y, col, (frac >> 8) & 0xFF);
			if (y + 1 < im->sy) {
				gdImageSetAAPixelColor(im, x, y + 1, col, (~frac >> 8) & 0xFF);
			}
			frac += inc;
			if (frac >= 65536) {
				frac -= 65536;
				y++;
			} else if (frac < 0) {
				frac += 65536;
				y--;
			}
		}
	} else {
		if (dy < 0) {
			tmp = x1;
			x1 = x2;
			x2 = tmp;
			tmp = y1;
			y1 = y2;
			y2 = tmp;
			dx = x2 - x1;
			dy = y2 - y1;
		}
		x = x1;
		inc = (dx * 65536) / dy;
		frac = 0;
		for (y = y1; y <= y2; y++) {
			gdImageSetAAPixelColor(im, x, y, col, (frac >> 8) & 0xFF);
			if (x + 1 < im->sx) {
				gdImageSetAAPixelColor(im, x + 1, y, col, (~frac >> 8) & 0xFF);
			}
			frac += inc;
			if (frac >= 65536) {
				frac -= 65536;
				x++;
			} else if (frac < 0) {
				frac += 65536;
				x--;
			}
		}
	}
}