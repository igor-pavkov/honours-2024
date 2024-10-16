BGD_DECLARE(int) gdImageColorResolve (gdImagePtr im, int r, int g, int b)
{
  return gdImageColorResolveAlpha (im, r, g, b, gdAlphaOpaque);
}