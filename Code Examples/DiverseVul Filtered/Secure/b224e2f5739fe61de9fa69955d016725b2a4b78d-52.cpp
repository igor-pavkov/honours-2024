SplashPattern *SplashOutputDev::getColor(GfxCMYK *cmyk) {
  SplashColor color;

  color[0] = colToByte(cmyk->c);
  color[1] = colToByte(cmyk->m);
  color[2] = colToByte(cmyk->y);
  color[3] = colToByte(cmyk->k);
  return new SplashSolidColor(color);
}