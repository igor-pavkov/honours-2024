static void clipColor(int rIn, int gIn, int bIn,
		      unsigned char *rOut, unsigned char *gOut, unsigned char *bOut) {
  int lum, rgbMin, rgbMax;

  lum = getLum(rIn, gIn, bIn);
  rgbMin = rgbMax = rIn;
  if (gIn < rgbMin) {
    rgbMin = gIn;
  } else if (gIn > rgbMax) {
    rgbMax = gIn;
  }
  if (bIn < rgbMin) {
    rgbMin = bIn;
  } else if (bIn > rgbMax) {
    rgbMax = bIn;
  }
  if (rgbMin < 0) {
    *rOut = (unsigned char)(lum + ((rIn - lum) * lum) / (lum - rgbMin));
    *gOut = (unsigned char)(lum + ((gIn - lum) * lum) / (lum - rgbMin));
    *bOut = (unsigned char)(lum + ((bIn - lum) * lum) / (lum - rgbMin));
  } else if (rgbMax > 255) {
    *rOut = (unsigned char)(lum + ((rIn - lum) * (255 - lum)) / (rgbMax - lum));
    *gOut = (unsigned char)(lum + ((gIn - lum) * (255 - lum)) / (rgbMax - lum));
    *bOut = (unsigned char)(lum + ((bIn - lum) * (255 - lum)) / (rgbMax - lum));
  } else {
    *rOut = rIn;
    *gOut = gIn;
    *bOut = bIn;
  }
}