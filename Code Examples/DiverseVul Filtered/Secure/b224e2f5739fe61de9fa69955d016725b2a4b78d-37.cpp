SplashPattern *SplashOutputDev::getColor(GfxColor *deviceN) {
  SplashColor color;

  for (int i = 0; i < 4 + SPOT_NCOMPS; i++)
    color[i] = colToByte(deviceN->c[i]);
  return new SplashSolidColor(color);
}