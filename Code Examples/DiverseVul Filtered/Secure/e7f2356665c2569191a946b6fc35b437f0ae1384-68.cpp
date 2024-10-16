int gdImageGetPixel (gdImagePtr im, int x, int y)
{
	if (gdImageBoundsSafe(im, x, y)) {
		if (im->trueColor) {
			return im->tpixels[y][x];
		} else {
			return im->pixels[y][x];
		}
	} else {
		return 0;
	}
}