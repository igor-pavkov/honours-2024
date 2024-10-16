static void gdImageTileApply (gdImagePtr im, int x, int y)
{
	gdImagePtr tile = im->tile;
	int srcx, srcy;
	int p;
	if (!tile) {
		return;
	}
	srcx = x % gdImageSX(tile);
	srcy = y % gdImageSY(tile);
	if (im->trueColor) {
		p = gdImageGetPixel(tile, srcx, srcy);
		if (p != gdImageGetTransparent (tile)) {
			if (!tile->trueColor) {
				p = gdTrueColorAlpha(tile->red[p], tile->green[p], tile->blue[p], tile->alpha[p]);
			}
			gdImageSetPixel(im, x, y, p);
		}
	} else {
		p = gdImageGetPixel(tile, srcx, srcy);
		/* Allow for transparency */
		if (p != gdImageGetTransparent(tile)) {
			if (tile->trueColor) {
				/* Truecolor tile. Very slow on a palette destination. */
				gdImageSetPixel(im, x, y, gdImageColorResolveAlpha(im,
											gdTrueColorGetRed(p),
											gdTrueColorGetGreen(p),
											gdTrueColorGetBlue(p),
											gdTrueColorGetAlpha(p)));
			} else {
				gdImageSetPixel(im, x, y, im->tileColorMap[p]);
			}
		}
	}
}