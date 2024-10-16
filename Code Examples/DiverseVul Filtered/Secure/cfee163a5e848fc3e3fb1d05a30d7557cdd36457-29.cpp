BGD_DECLARE(int) gdImageColorExactAlpha (gdImagePtr im, int r, int g, int b, int a)
{
  int i;
  if (im->trueColor)
    {
      return gdTrueColorAlpha (r, g, b, a);
    }
  for (i = 0; (i < (im->colorsTotal)); i++)
    {
      if (im->open[i])
	{
	  continue;
	}
      if ((im->red[i] == r) &&
	  (im->green[i] == g) && (im->blue[i] == b) && (im->alpha[i] == a))
	{
	  return i;
	}
    }
  return -1;
}