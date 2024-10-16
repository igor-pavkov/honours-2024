BGD_DECLARE(void) gdImageSetStyle (gdImagePtr im, int *style, int noOfPixels)
{
  if (im->style)
    {
      gdFree (im->style);
    }
  if (overflow2(sizeof (int), noOfPixels)) {
    return;
  }   	
  im->style = (int *) gdMalloc (sizeof (int) * noOfPixels);
  memcpy (im->style, style, sizeof (int) * noOfPixels);
  im->styleLength = noOfPixels;
  im->stylePos = 0;
}