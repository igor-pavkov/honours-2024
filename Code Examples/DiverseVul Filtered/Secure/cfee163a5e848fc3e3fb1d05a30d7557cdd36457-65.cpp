BGD_DECLARE(void) gdImageDestroy (gdImagePtr im)
{
  int i;
  if (im->pixels)
    {
      for (i = 0; (i < im->sy); i++)
	{
	  gdFree (im->pixels[i]);
	}
      gdFree (im->pixels);
    }
  if (im->tpixels)
    {
      for (i = 0; (i < im->sy); i++)
	{
	  gdFree (im->tpixels[i]);
	}
      gdFree (im->tpixels);
    }
  if (im->polyInts)
    {
      gdFree (im->polyInts);
    }
  if (im->style)
    {
      gdFree (im->style);
    }
  gdFree (im);
}