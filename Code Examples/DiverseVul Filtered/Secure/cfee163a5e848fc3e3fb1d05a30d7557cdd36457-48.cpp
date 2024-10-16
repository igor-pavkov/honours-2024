BGD_DECLARE(int) gdImageColorAllocateAlpha (gdImagePtr im, int r, int g, int b, int a)
{
  int i;
  int ct = (-1);
  if (im->trueColor)
    {
      return gdTrueColorAlpha (r, g, b, a);
    }
  for (i = 0; (i < (im->colorsTotal)); i++)
    {
      if (im->open[i])
	{
	  ct = i;
	  break;
	}
    }
  if (ct == (-1))
    {
      ct = im->colorsTotal;
      if (ct == gdMaxColors)
	{
	  return -1;
	}
      im->colorsTotal++;
    }
  im->red[ct] = r;
  im->green[ct] = g;
  im->blue[ct] = b;
  im->alpha[ct] = a;
  im->open[ct] = 0;
  return ct;
}