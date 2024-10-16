static inline void convertGfxColor(SplashColorPtr dest,
                                   SplashColorMode colorMode,
                                   GfxColorSpace *colorSpace,
                                   GfxColor *src) {
  SplashColor color;
  GfxGray gray;
  GfxRGB rgb;
#ifdef SPLASH_CMYK
  GfxCMYK cmyk;
  GfxColor deviceN;
#endif

  // make gcc happy
  color[0] = color[1] = color[2] = 0;
#ifdef SPLASH_CMYK
  color[3] = 0;
#endif
  switch (colorMode) {
    case splashModeMono1:
    case splashModeMono8:
      colorSpace->getGray(src, &gray);
      color[0] = colToByte(gray);
    break;
    case splashModeXBGR8:
      color[3] = 255;
      // fallthrough
    case splashModeBGR8:
    case splashModeRGB8:
      colorSpace->getRGB(src, &rgb);
      color[0] = colToByte(rgb.r);
      color[1] = colToByte(rgb.g);
      color[2] = colToByte(rgb.b);
    break;
#ifdef SPLASH_CMYK
    case splashModeCMYK8:
      colorSpace->getCMYK(src, &cmyk);
      color[0] = colToByte(cmyk.c);
      color[1] = colToByte(cmyk.m);
      color[2] = colToByte(cmyk.y);
      color[3] = colToByte(cmyk.k);
    break;
    case splashModeDeviceN8:
      colorSpace->getDeviceN(src, &deviceN);
      for (int i = 0; i < SPOT_NCOMPS + 4; i++)
        color[i] = colToByte(deviceN.c[i]);
    break;
#endif
  }
  splashColorCopy(dest, color);
}