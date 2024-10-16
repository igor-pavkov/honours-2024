static void gdImageBrushApply (gdImagePtr im, int x, int y)
{
	int lx, ly;
	int hy, hx;
	int x1, y1, x2, y2;
	int srcx, srcy;

	if (!im->brush) {
		return;
	}

	hy = gdImageSY(im->brush) / 2;
	y1 = y - hy;
	y2 = y1 + gdImageSY(im->brush);
	hx = gdImageSX(im->brush) / 2;
	x1 = x - hx;
	x2 = x1 + gdImageSX(im->brush);
	srcy = 0;

	if (im->trueColor) {
		if (im->brush->trueColor) {
			for (ly = y1; ly < y2; ly++) {
				srcx = 0;
				for (lx = x1; (lx < x2); lx++) {
					int p;
					p = gdImageGetTrueColorPixel(im->brush, srcx, srcy);
					/* 2.0.9, Thomas Winzig: apply simple full transparency */
					if (p != gdImageGetTransparent(im->brush)) {
						gdImageSetPixel(im, lx, ly, p);
					}
					srcx++;
				}
				srcy++;
			}
		} else {
			/* 2.0.12: Brush palette, image truecolor (thanks to Thorben Kundinger for pointing out the issue) */
			for (ly = y1; ly < y2; ly++) {
				srcx = 0;
				for (lx = x1; lx < x2; lx++) {
					int p, tc;
					p = gdImageGetPixel(im->brush, srcx, srcy);
					tc = gdImageGetTrueColorPixel(im->brush, srcx, srcy);
					/* 2.0.9, Thomas Winzig: apply simple full transparency */
					if (p != gdImageGetTransparent(im->brush)) {
						gdImageSetPixel(im, lx, ly, tc);
					}
					srcx++;
				}
				srcy++;
			}
		}
	} else {
		for (ly = y1; ly < y2; ly++) {
			srcx = 0;
			for (lx = x1; lx < x2; lx++) {
				int p;
				p = gdImageGetPixel(im->brush, srcx, srcy);
				/* Allow for non-square brushes! */
				if (p != gdImageGetTransparent(im->brush)) {
					/* Truecolor brush. Very slow on a palette destination. */
					if (im->brush->trueColor) {
						gdImageSetPixel(im, lx, ly, gdImageColorResolveAlpha(im, gdTrueColorGetRed(p),
													 gdTrueColorGetGreen(p),
													 gdTrueColorGetBlue(p),
													 gdTrueColorGetAlpha(p)));
					} else {
						gdImageSetPixel(im, lx, ly, im->brushColorMap[p]);
					}
				}
				srcx++;
			}
			srcy++;
		}
	}
}