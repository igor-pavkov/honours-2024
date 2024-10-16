BGD_DECLARE(void) gdImageFilledRectangle (gdImagePtr im, int x1, int y1, int x2, int y2,
			int color)
{
  int x, y;
  /* Nick Atty: limit the points at the edge.  Note that this also
     nicely kills any plotting for rectangles completely outside the
     window as it makes the tests in the for loops fail */
  if (x1 < 0)
    x1 = 0;
  if (x1 > gdImageSX (im))
    x1 = gdImageSX (im);
  if (y1 < 0)
    y1 = 0;
  if (y1 > gdImageSY (im))
    y1 = gdImageSY (im);

  for (y = y1; (y <= y2); y++)
    {
      for (x = x1; (x <= x2); x++)
	{
	  gdImageSetPixel (im, x, y, color);
	}
    }
}