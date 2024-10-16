void SplashGouraudPattern::getParameterizedColor(double colorinterp, SplashColorMode mode, SplashColorPtr dest) {
  GfxColor src;
  GfxColorSpace* srcColorSpace = shading->getColorSpace();
  int colorComps = 3;
#ifdef SPLASH_CMYK
  if (mode == splashModeCMYK8)
    colorComps=4;
  else if (mode == splashModeDeviceN8)
    colorComps=4 + SPOT_NCOMPS;
#endif

  shading->getParameterizedColor(colorinterp, &src);

  if (bDirectColorTranslation) {
    for (int m = 0; m < colorComps; ++m)
      dest[m] = colToByte(src.c[m]);
  } else {
    convertGfxShortColor(dest, mode, srcColorSpace, &src);
  }
}