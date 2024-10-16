void SplashOutputDev::updateStrokeColor(GfxState *state) {
  GfxGray gray;
  GfxRGB rgb;
#ifdef SPLASH_CMYK
  GfxCMYK cmyk;
  GfxColor deviceN;
#endif

  switch (colorMode) {
  case splashModeMono1:
  case splashModeMono8:
    state->getStrokeGray(&gray);
    splash->setStrokePattern(getColor(gray));
    break;
  case splashModeXBGR8:
  case splashModeRGB8:
  case splashModeBGR8:
    state->getStrokeRGB(&rgb);
    splash->setStrokePattern(getColor(&rgb));
    break;
#ifdef SPLASH_CMYK
  case splashModeCMYK8:
    state->getStrokeCMYK(&cmyk);
    splash->setStrokePattern(getColor(&cmyk));
    break;
  case splashModeDeviceN8:
    state->getStrokeDeviceN(&deviceN);
    splash->setStrokePattern(getColor(&deviceN));
    break;
#endif
  }
}