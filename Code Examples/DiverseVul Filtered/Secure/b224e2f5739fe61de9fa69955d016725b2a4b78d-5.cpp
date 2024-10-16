static void splashOutBlendSoftLight(SplashColorPtr src, SplashColorPtr dest,
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
      if (src[i] < 0x80) {
        blend[i] = dest[i] - (255 - 2 * src[i]) * dest[i] * (255 - dest[i]) / (255 * 255);
      } else {
        if (dest[i] < 0x40) {
          x = (((((16 * dest[i] - 12 * 255) * dest[i]) / 255) + 4 * 255) * dest[i]) / 255;
        } else {
          x = (int)sqrt(255.0 * dest[i]);
        }
        blend[i] = dest[i] + (2 * src[i] - 255) * (x - dest[i]) / 255;
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