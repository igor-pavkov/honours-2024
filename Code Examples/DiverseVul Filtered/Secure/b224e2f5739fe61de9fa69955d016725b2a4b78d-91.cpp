SplashPattern *SplashOutputDev::getColor(GfxRGB *rgb) {
  GfxColorComp r, g, b;
  SplashColor color;

  if (reverseVideo) {
    r = gfxColorComp1 - rgb->r;
    g = gfxColorComp1 - rgb->g;
    b = gfxColorComp1 - rgb->b;
  } else {
    r = rgb->r;
    g = rgb->g;
    b = rgb->b;
  }
  color[0] = colToByte(r);
  color[1] = colToByte(g);
  color[2] = colToByte(b);
  if (colorMode == splashModeXBGR8) color[3] = 255;
  return new SplashSolidColor(color);
}