void gdImagePolygon (gdImagePtr im, gdPointPtr p, int n, int c)
{
	int i;
	int lx, ly;
	typedef void (*image_line)(gdImagePtr im, int x1, int y1, int x2, int y2, int color);
	image_line draw_line;

	if (n <= 0) {
		return;
	}

	/* Let it be known that we are drawing a polygon so that the opacity
	 * mask doesn't get cleared after each line.
	 */
	if (c == gdAntiAliased) {
		im->AA_polygon = 1;
	}

	if ( im->antialias) {
		draw_line = gdImageAALine;
	} else {
		draw_line = gdImageLine;
	}
	lx = p->x;
	ly = p->y;
	draw_line(im, lx, ly, p[n - 1].x, p[n - 1].y, c);
	for (i = 1; i < n; i++) {
		p++;
		draw_line(im, lx, ly, p->x, p->y, c);
		lx = p->x;
		ly = p->y;
	}

	if (c == gdAntiAliased) {
		im->AA_polygon = 0;
		gdImageAABlend(im);
	}
}