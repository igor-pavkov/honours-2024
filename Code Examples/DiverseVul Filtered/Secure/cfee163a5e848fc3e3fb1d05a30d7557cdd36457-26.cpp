BGD_DECLARE(void) gdImageColorDeallocate (gdImagePtr im, int color)
{
  if (im->trueColor)
    {
      return;
    }
  /* Mark it open. */
  im->open[color] = 1;
}