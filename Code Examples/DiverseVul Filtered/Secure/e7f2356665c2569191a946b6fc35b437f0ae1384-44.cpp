void gdImageAALine (gdImagePtr im, int x1, int y1, int x2, int y2, int col)
{
	/* keep them as 32bits */
	long x, y, inc;
	long dx, dy,tmp;

	if (y1 < 0 && y2 < 0) {
		return;
	}
	if (y1 < 0) {
		x1 += (y1 * (x1 - x2)) / (y2 - y1);
		y1 = 0;
	}
	if (y2 < 0) {
		x2 += (y2 * (x1 - x2)) / (y2 - y1);
		y2 = 0;
	}

	/* bottom edge */
	if (y1 >= im->sy && y2 >= im->sy) {
		return;
	}
	if (y1 >= im->sy) {
		x1 -= ((im->sy - y1) * (x1 - x2)) / (y2 - y1);
		y1 = im->sy - 1;
	}
	if (y2 >= im->sy) {
		x2 -= ((im->sy - y2) * (x1 - x2)) / (y2 - y1);
		y2 = im->sy - 1;
	}

	/* left edge */
	if (x1 < 0 && x2 < 0) {
		return;
	}
	if (x1 < 0) {
		y1 += (x1 * (y1 - y2)) / (x2 - x1);
		x1 = 0;
	}
	if (x2 < 0) {
		y2 += (x2 * (y1 - y2)) / (x2 - x1);
		x2 = 0;
	}
	/* right edge */
	if (x1 >= im->sx && x2 >= im->sx) {
		return;
	}
	if (x1 >= im->sx) {
		y1 -= ((im->sx - x1) * (y1 - y2)) / (x2 - x1);
		x1 = im->sx - 1;
	}
	if (x2 >= im->sx) {
		y2 -= ((im->sx - x2) * (y1 - y2)) / (x2 - x1);
		x2 = im->sx - 1;
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
		x = x1 << 16;
		y = y1 << 16;
		inc = (dy * 65536) / dx;
		while ((x >> 16) <= x2) {
			gdImageSetAAPixelColor(im, x >> 16, y >> 16, col, (y >> 8) & 0xFF);
			if ((y >> 16) + 1 < im->sy) {
				gdImageSetAAPixelColor(im, x >> 16, (y >> 16) + 1,col, (~y >> 8) & 0xFF);
			}
			x += (1 << 16);
			y += inc;
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
		x = x1 << 16;
		y = y1 << 16;
		inc = (dx * 65536) / dy;
		while ((y>>16) <= y2) {
			gdImageSetAAPixelColor(im, x >> 16, y >> 16, col, (x >> 8) & 0xFF);
			if ((x >> 16) + 1 < im->sx) {
				gdImageSetAAPixelColor(im, (x >> 16) + 1, (y >> 16),col, (~x >> 8) & 0xFF);
			}
			x += inc;
			y += (1<<16);
		}
	}
}