BGD_DECLARE(int) gdImageColorAllocate (gdImagePtr im, int r, int g, int b)
{
  return gdImageColorAllocateAlpha (im, r, g, b, gdAlphaOpaque);
}