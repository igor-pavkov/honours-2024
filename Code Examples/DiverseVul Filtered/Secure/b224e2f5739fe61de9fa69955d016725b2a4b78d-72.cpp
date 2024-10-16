static void setLum(unsigned char rIn, unsigned char gIn, unsigned char bIn, int lum,
		   unsigned char *rOut, unsigned char *gOut, unsigned char *bOut) {
  int d;

  d = lum - getLum(rIn, gIn, bIn);
  clipColor(rIn + d, gIn + d, bIn + d, rOut, gOut, bOut);
}