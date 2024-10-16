SplashPattern *SplashOutputDev::getColor(GfxGray gray) {
  SplashColor color;

  if (reverseVideo) {
    gray = gfxColorComp1 - gray;
  }
  color[0] = colToByte(gray);
  return new SplashSolidColor(color);
}