BGD_DECLARE(void) gdImageFilledArc (gdImagePtr im, int cx, int cy, int w, int h, int s, int e,
									int color, int style)
{
	gdPoint pts[363];
	int i, pti;
	int lx = 0, ly = 0;
	int fx = 0, fy = 0;

	if ((s % 360)  == (e % 360)) {
		s = 0;
		e = 360;
	} else {
		if (s > 360) {
			s = s % 360;
		}

		if (e > 360) {
			e = e % 360;
		}

		while (s < 0) {
			s += 360;
		}

		while (e < s) {
			e += 360;
		}

		if (s == e) {
			s = 0;
			e = 360;
		}
	}

	for (i = s, pti = 1; (i <= e); i++, pti++) {
		int x, y;
		x = ((long) gdCosT[i % 360] * (long) w / (2 * 1024)) + cx;
		y = ((long) gdSinT[i % 360] * (long) h / (2 * 1024)) + cy;
		if (i != s) {
			if (!(style & gdChord)) {
				if (style & gdNoFill) {
					gdImageLine (im, lx, ly, x, y, color);
				} else {
					if (y == ly) {
						pti--; /* don't add this point */
						if (((i > 270 || i < 90) && x > lx) || ((i >  90 && i < 270) && x < lx)) {
							/* replace the old x coord, if increasing on the
							   right side or decreasing on the left side */
							pts[pti].x = x;
						}
					} else {
						pts[pti].x = x;
						pts[pti].y = y;
					}
				}
			}
		} else {
			fx = x;
			fy = y;

			if (!(style & (gdChord | gdNoFill))) {
				pts[0].x = cx;
				pts[0].y = cy;
				pts[pti].x = x;
				pts[pti].y = y;
			}
		}
		lx = x;
		ly = y;
	}
	if (style & gdChord) {
		if (style & gdNoFill) {
			if (style & gdEdged) {
				gdImageLine (im, cx, cy, lx, ly, color);
				gdImageLine (im, cx, cy, fx, fy, color);
			}
			gdImageLine (im, fx, fy, lx, ly, color);
		} else {
			pts[0].x = fx;
			pts[0].y = fy;
			pts[1].x = lx;
			pts[1].y = ly;
			pts[2].x = cx;
			pts[2].y = cy;
			gdImageFilledPolygon (im, pts, 3, color);
		}
	} else {
		if (style & gdNoFill) {
			if (style & gdEdged) {
				gdImageLine (im, cx, cy, lx, ly, color);
				gdImageLine (im, cx, cy, fx, fy, color);
			}
		} else {
			pts[pti].x = cx;
			pts[pti].y = cy;
			gdImageFilledPolygon(im, pts, pti+1, color);
		}
	}
}