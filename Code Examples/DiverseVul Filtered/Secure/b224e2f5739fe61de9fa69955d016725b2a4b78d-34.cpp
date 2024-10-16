static void setSat(unsigned char rIn, unsigned char gIn, unsigned char bIn, int sat,
		   unsigned char *rOut, unsigned char *gOut, unsigned char *bOut) {
  int rgbMin, rgbMid, rgbMax;
  unsigned char *minOut, *midOut, *maxOut;

  if (rIn < gIn) {
    rgbMin = rIn;  minOut = rOut;
    rgbMid = gIn;  midOut = gOut;
  } else {
    rgbMin = gIn;  minOut = gOut;
    rgbMid = rIn;  midOut = rOut;
  }
  if (bIn > rgbMid) {
    rgbMax = bIn;  maxOut = bOut;
  } else if (bIn > rgbMin) {
    rgbMax = rgbMid;  maxOut = midOut;
    rgbMid = bIn;     midOut = bOut;
  } else {
    rgbMax = rgbMid;  maxOut = midOut;
    rgbMid = rgbMin;  midOut = minOut;
    rgbMin = bIn;     minOut = bOut;
  }
  if (rgbMax > rgbMin) {
    *midOut = (unsigned char)((rgbMid - rgbMin) * sat) / (rgbMax - rgbMin);
    *maxOut = (unsigned char)sat;
  } else {
    *midOut = *maxOut = 0;
  }
  *minOut = 0;
}