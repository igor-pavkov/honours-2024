static int gdImageTileGet (gdImagePtr im, int x, int y)
{
	int srcx, srcy;
	int tileColor,p;
	if (!im->tile) {
		return -1;
	}
	srcx = x % gdImageSX(im->tile);
	srcy = y % gdImageSY(im->tile);
	p = gdImageGetPixel(im->tile, srcx, srcy);

	if (im->trueColor) {
		if (im->tile->trueColor) {
			tileColor = p;
		} else {
			tileColor = gdTrueColorAlpha( gdImageRed(im->tile,p), gdImageGreen(im->tile,p), gdImageBlue (im->tile,p), gdImageAlpha (im->tile,p));
		}
	} else {
		if (im->tile->trueColor) {
			tileColor = gdImageColorResolveAlpha(im, gdTrueColorGetRed (p), gdTrueColorGetGreen (p), gdTrueColorGetBlue (p), gdTrueColorGetAlpha (p));
		} else {
			tileColor = p;
			tileColor = gdImageColorResolveAlpha(im, gdImageRed (im->tile,p), gdImageGreen (im->tile,p), gdImageBlue (im->tile,p), gdImageAlpha (im->tile,p));
		}
	}
	return tileColor;
}