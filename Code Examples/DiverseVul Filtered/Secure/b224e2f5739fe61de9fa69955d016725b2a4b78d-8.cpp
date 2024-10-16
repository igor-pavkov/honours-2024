void SplashOutputDev::getMatteColor(SplashColorMode colorMode, GfxImageColorMap *colorMap, const GfxColor *matteColorIn, SplashColor matteColor) {
  GfxGray gray;
  GfxRGB rgb;
#ifdef SPLASH_CMYK
  GfxCMYK cmyk;
  GfxColor deviceN;
#endif

  switch (colorMode) {
    case splashModeMono1:
    case splashModeMono8:
      colorMap->getColorSpace()->getGray(matteColorIn, &gray);
      matteColor[0] = colToByte(gray);
      break;
    case splashModeRGB8:
    case splashModeBGR8:
      colorMap->getColorSpace()->getRGB(matteColorIn, &rgb);
      matteColor[0] = colToByte(rgb.r);
      matteColor[1] = colToByte(rgb.g);
      matteColor[2] = colToByte(rgb.b);
      break;
    case splashModeXBGR8:
      colorMap->getColorSpace()->getRGB(matteColorIn, &rgb);
      matteColor[0] = colToByte(rgb.r);
      matteColor[1] = colToByte(rgb.g);
      matteColor[2] = colToByte(rgb.b);
      matteColor[3] = 255;
      break;
#ifdef SPLASH_CMYK
    case splashModeCMYK8:
      colorMap->getColorSpace()->getCMYK(matteColorIn, &cmyk);
      matteColor[0] = colToByte(cmyk.c);
      matteColor[1] = colToByte(cmyk.m);
      matteColor[2] = colToByte(cmyk.y);
      matteColor[3] = colToByte(cmyk.k);
      break;
    case splashModeDeviceN8:
      colorMap->getColorSpace()->getDeviceN(matteColorIn, &deviceN);
      for (int cp = 0; cp < SPOT_NCOMPS+4; cp++)
        matteColor[cp] = colToByte(deviceN.c[cp]);
      break;
#endif
  }
}