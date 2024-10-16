static int getSat(int r, int g, int b) {
  int rgbMin, rgbMax;

  rgbMin = rgbMax = r;
  if (g < rgbMin) {
    rgbMin = g;
  } else if (g > rgbMax) {
    rgbMax = g;
  }
  if (b < rgbMin) {
    rgbMin = b;
  } else if (b > rgbMax) {
    rgbMax = b;
  }
  return rgbMax - rgbMin;
}