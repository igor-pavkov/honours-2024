BGD_DECLARE(gdImagePtr) gdImageCreateTrueColor (int sx, int sy)
{
  int i;
  gdImagePtr im;
  im = (gdImage *) gdMalloc (sizeof (gdImage));
  memset (im, 0, sizeof (gdImage));
  im->tpixels = (int **) gdMalloc (sizeof (int *) * sy);
  im->polyInts = 0;
  im->polyAllocated = 0;
  im->brush = 0;
  im->tile = 0;
  im->style = 0;
  for (i = 0; (i < sy); i++)
    {
      im->tpixels[i] = (int *) gdCalloc (sx, sizeof (int));
    }
  im->sx = sx;
  im->sy = sy;
  im->transparent = (-1);
  im->interlace = 0;
  im->trueColor = 1;
  /* 2.0.2: alpha blending is now on by default, and saving of alpha is
     off by default. This allows font antialiasing to work as expected
     on the first try in JPEGs -- quite important -- and also allows 
     for smaller PNGs when saving of alpha channel is not really 
     desired, which it usually isn't! */
  im->saveAlphaFlag = 0;
  im->alphaBlendingFlag = 1;
  im->thick = 1;
  im->AA = 0;
  im->cx1 = 0;
  im->cy1 = 0;
  im->cx2 = im->sx - 1;
  im->cy2 = im->sy - 1;
  return im;
}