BGD_DECLARE(int) gdImageColorExact (gdImagePtr im, int r, int g, int b)
{
  return gdImageColorExactAlpha (im, r, g, b, gdAlphaOpaque);
}