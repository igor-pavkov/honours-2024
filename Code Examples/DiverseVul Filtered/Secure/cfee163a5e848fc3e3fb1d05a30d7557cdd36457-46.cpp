BGD_DECLARE(int) gdImageColorClosest (gdImagePtr im, int r, int g, int b)
{
  return gdImageColorClosestAlpha (im, r, g, b, gdAlphaOpaque);
}