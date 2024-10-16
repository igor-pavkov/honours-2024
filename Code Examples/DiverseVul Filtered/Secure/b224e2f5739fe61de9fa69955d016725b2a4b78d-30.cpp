static void splashOutBlendDifference(SplashColorPtr src, SplashColorPtr dest,
				     SplashColorPtr blend,
				     SplashColorMode cm) {
  int i;

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
      blend[i] = dest[i] < src[i] ? src[i] - dest[i] : dest[i] - src[i];
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
  if (cm == splashModeDeviceN8) {
    for (i = 4; i < splashColorModeNComps[cm]; ++i) {
      if (dest[i] == 0 && src[i] == 0)
        blend[i] = 0;
    }
  }
#endif
}