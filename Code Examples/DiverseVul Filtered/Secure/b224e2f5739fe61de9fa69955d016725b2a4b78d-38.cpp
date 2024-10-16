void SplashOutputDev::updateFillColor(GfxState *state) {
  GfxGray gray;
  GfxRGB rgb;
#ifdef SPLASH_CMYK
  GfxCMYK cmyk;
  GfxColor deviceN;
#endif

  switch (colorMode) {
  case splashModeMono1:
  case splashModeMono8:
    state->getFillGray(&gray);
    splash->setFillPattern(getColor(gray));
    break;
  case splashModeXBGR8:
  case splashModeRGB8:
  case splashModeBGR8:
    state->getFillRGB(&rgb);
    splash->setFillPattern(getColor(&rgb));
    break;
#ifdef SPLASH_CMYK
  case splashModeCMYK8:
    state->getFillCMYK(&cmyk);
    splash->setFillPattern(getColor(&cmyk));
    break;
  case splashModeDeviceN8:
    state->getFillDeviceN(&deviceN);
    splash->setFillPattern(getColor(&deviceN));
    break;
#endif
  }
}