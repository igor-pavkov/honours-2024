void gdImageColorTransparent (gdImagePtr im, int color)
{
	if (!im->trueColor) {
		if (im->transparent != -1) {
			im->alpha[im->transparent] = gdAlphaOpaque;
		}
		if (color > -1 && color < im->colorsTotal && color < gdMaxColors) {
			im->alpha[color] = gdAlphaTransparent;
		} else {
			return;
		}
	}
	im->transparent = color;
}