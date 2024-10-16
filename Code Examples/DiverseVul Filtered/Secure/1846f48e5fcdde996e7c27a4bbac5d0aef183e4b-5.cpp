BGD_DECLARE(void) gdImageFilledRectangle (gdImagePtr im, int x1, int y1, int x2, int y2,
		int color)
{
	_gdImageFilledVRectangle(im, x1, y1, x2, y2, color);
}