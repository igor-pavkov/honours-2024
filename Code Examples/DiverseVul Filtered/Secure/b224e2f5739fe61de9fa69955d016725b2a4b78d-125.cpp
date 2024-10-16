static void splashOutBlendColorDodge(SplashColorPtr src, SplashColorPtr dest,
				     SplashColorPtr blend,
				     SplashColorMode cm) {
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
      if (src[i] == 255) {
        blend[i] = 255;
      } else {
        x = (dest[i] * 255) / (255 - src[i]);
        blend[i] = x <= 255 ? x : 255;
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