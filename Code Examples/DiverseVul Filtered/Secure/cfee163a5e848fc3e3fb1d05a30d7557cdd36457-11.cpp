BGD_DECLARE(int) gdImageColorClosestHWB (gdImagePtr im, int r, int g, int b)
{
  int i;
  /* long rd, gd, bd; */
  int ct = (-1);
  int first = 1;
  float mindist = 0;
  if (im->trueColor)
    {
      return gdTrueColor (r, g, b);
    }
  for (i = 0; (i < (im->colorsTotal)); i++)
    {
      float dist;
      if (im->open[i])
	{
	  continue;
	}
      dist = HWB_Diff (im->red[i], im->green[i], im->blue[i], r, g, b);
      if (first || (dist < mindist))
	{
	  mindist = dist;
	  ct = i;
	  first = 0;
	}
    }
  return ct;
}