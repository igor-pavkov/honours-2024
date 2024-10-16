BGD_DECLARE(int) gdImageColorClosestAlpha (gdImagePtr im, int r, int g, int b, int a)
{
  int i;
  long rd, gd, bd, ad;
  int ct = (-1);
  int first = 1;
  long mindist = 0;
  if (im->trueColor)
    {
      return gdTrueColorAlpha (r, g, b, a);
    }
  for (i = 0; (i < (im->colorsTotal)); i++)
    {
      long dist;
      if (im->open[i])
	{
	  continue;
	}
      rd = (im->red[i] - r);
      gd = (im->green[i] - g);
      bd = (im->blue[i] - b);
      /* gd 2.02: whoops, was - b (thanks to David Marwood) */
      /* gd 2.16: was blue rather than alpha! Geez! Thanks to 
         Artur Jakub Jerzak */
      ad = (im->alpha[i] - a);
      dist = rd * rd + gd * gd + bd * bd + ad * ad;
      if (first || (dist < mindist))
	{
	  mindist = dist;
	  ct = i;
	  first = 0;
	}
    }
  return ct;
}