void gdImageArc (gdImagePtr im, int cx, int cy, int w, int h, int s, int e, int color)
{
	if ((s % 360) == (e % 360)) {
		gdImageEllipse(im, cx, cy, w, h, color);
	} else {
		gdImageFilledArc(im, cx, cy, w, h, s, e, color, gdNoFill);
	}
}