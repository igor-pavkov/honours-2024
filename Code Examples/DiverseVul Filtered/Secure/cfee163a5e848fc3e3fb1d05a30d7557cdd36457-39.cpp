BGD_DECLARE(int) gdImageGetTrueColorPixel (gdImagePtr im, int x, int y)
{
  int p = gdImageGetPixel (im, x, y);
  if (!im->trueColor)
    {
      return gdTrueColorAlpha (im->red[p], im->green[p], im->blue[p],
			       (im->transparent == p) ? gdAlphaTransparent :
			       im->alpha[p]);
    }
  else
    {
      return p;
    }
}