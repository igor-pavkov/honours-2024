static void horizontalLine(gdImagePtr im, int minx, int maxx, int y,
	int fill_color)
{
  /* 2.0.27: potential corruption fixed by John Ellson */
  if (minx < im->cx1) minx = im->cx1;
  if (maxx < minx) maxx = minx;
  if (maxx > im->cx2) maxx = im->cx2;
  if (minx > maxx) minx = maxx;
                                                                                                                                        
  if (y < im->cy1) y = im->cy1;
  if (y > im->cy2) y = im->cy2;
                                                                                                                                        
  if (im->trueColor) {
    while (minx <= maxx) {
      im->tpixels[y][minx++] = fill_color;
    }
  } else {
    while (minx <= maxx) {
      im->pixels[y][minx++] = fill_color;
    }
  }
}