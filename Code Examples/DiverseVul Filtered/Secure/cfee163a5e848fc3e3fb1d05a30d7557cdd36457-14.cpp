BGD_DECLARE(void) gdImageArc (gdImagePtr im, int cx, int cy, int w, int h, int s, int e,
	    int color)
{
  gdImageFilledArc (im, cx, cy, w, h, s, e, color, gdNoFill);
}