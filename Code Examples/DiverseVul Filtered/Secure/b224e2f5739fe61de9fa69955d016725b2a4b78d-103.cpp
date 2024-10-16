static void splashOutBlendColorBurn(SplashColorPtr src, SplashColorPtr dest,
				    SplashColorPtr blend, SplashColorMode cm) {
  int i, x;

#ifdef SPLASH_CMYK
  if (cm == splashModeCMYK8 || cm == splashModeDeviceN8) {
    for (i = 0; i < splashColorModeNComps[cm]; ++i) {
      dest[i] = 255 - dest[i];
      src[i] = 255 - src[i];
    }
  }
#endif
  {
    for (i = 0; i < splashColorModeNComps[cm]; ++i) {
      if (src[i] == 0) {
        blend[i] = 0;
      } else {
        x = ((255 - dest[i]) * 255) / src[i];
        blend[i] = x <= 255 ? 255 - x : 0;
      }
    }
  }
#ifdef SPLASH_CMYK
  if (cm == splashModeCMYK8 || cm == splashModeDeviceN8) {
    for (i = 0; i < splashColorModeNComps[cm]; ++i) {
      dest[i] = 255 - dest[i];
      src[i] = 255 - src[i];
      blend[i] = 255 - blend[i];
    }
  }
#endif
}