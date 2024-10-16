static inline void convertGfxShortColor(SplashColorPtr dest,
                                   SplashColorMode colorMode,
                                   GfxColorSpace *colorSpace,
                                   GfxColor *src) {
  switch (colorMode) {
    case splashModeMono1:
    case splashModeMono8:
    {
      GfxGray gray;
      colorSpace->getGray(src, &gray);
      dest[0] = colToByte(gray);
    }
    break;
    case splashModeXBGR8:
      dest[3] = 255;
      // fallthrough
    case splashModeBGR8:
    case splashModeRGB8:
    {
      GfxRGB rgb;
      colorSpace->getRGB(src, &rgb);
      dest[0] = colToByte(rgb.r);
      dest[1] = colToByte(rgb.g);
      dest[2] = colToByte(rgb.b);
    }
    break;
#ifdef SPLASH_CMYK
    case splashModeCMYK8:
    {
      GfxCMYK cmyk;
      colorSpace->getCMYK(src, &cmyk);
      dest[0] = colToByte(cmyk.c);
      dest[1] = colToByte(cmyk.m);
      dest[2] = colToByte(cmyk.y);
      dest[3] = colToByte(cmyk.k);
    }
    break;
    case splashModeDeviceN8:
    {
      GfxColor deviceN;
      colorSpace->getDeviceN(src, &deviceN);
      for (int i = 0; i < SPOT_NCOMPS + 4; i++)
        dest[i] = colToByte(deviceN.c[i]);
    }
    break;
#endif
  }
}