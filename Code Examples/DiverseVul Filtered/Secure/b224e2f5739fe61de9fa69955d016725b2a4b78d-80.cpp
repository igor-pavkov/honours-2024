bool SplashOutputDev::tilingBitmapSrc(void *data, SplashColorPtr colorLine,
                                       unsigned char *alphaLine) {
  TilingSplashOutBitmap *imgData = (TilingSplashOutBitmap *)data;

  if (imgData->y == imgData->bitmap->getHeight()) {
    imgData->repeatY--;
    if (imgData->repeatY == 0)
      return false;
    imgData->y = 0;
  }

  if (imgData->paintType == 1) {
    const SplashColorMode cMode = imgData->bitmap->getMode();
    SplashColorPtr q = colorLine;
    // For splashModeBGR8 and splashModeXBGR8 we need to use getPixel
    // for the others we can use raw access
    if (cMode == splashModeBGR8 || cMode == splashModeXBGR8) {
      for (int m = 0; m < imgData->repeatX; m++) {
        for (int x = 0; x < imgData->bitmap->getWidth(); x++) {
          imgData->bitmap->getPixel(x, imgData->y, q);
          q += splashColorModeNComps[cMode];
        }
      }
    } else {
      const int n = imgData->bitmap->getRowSize();
      SplashColorPtr p;
      for (int m = 0; m < imgData->repeatX; m++) {
        p = imgData->bitmap->getDataPtr() + imgData->y * imgData->bitmap->getRowSize();
        for (int x = 0; x < n; ++x) {
          *q++ = *p++;
        }
      }
    }
    if (alphaLine != nullptr) {
      SplashColorPtr aq = alphaLine;
      SplashColorPtr p;
      const int n = imgData->bitmap->getWidth() - 1;
      for (int m = 0; m < imgData->repeatX; m++) {
        p = imgData->bitmap->getAlphaPtr() + imgData->y * imgData->bitmap->getWidth();
        for (int x = 0; x < n; ++x) {
          *aq++ = *p++;
        }
        // This is a hack, because of how Splash antialias works if we overwrite the
        // last alpha pixel of the tile most/all of the files look much better
        *aq++ = (n == 0) ? *p : *(p - 1);
      }
    }
  } else {
    SplashColor col, pat;
    SplashColorPtr dest = colorLine;
    for (int m = 0; m < imgData->repeatX; m++) {
      for (int x = 0; x < imgData->bitmap->getWidth(); x++) {
        imgData->bitmap->getPixel(x, imgData->y, col);
        imgData->pattern->getColor(x, imgData->y, pat);
        for (int i = 0; i < splashColorModeNComps[imgData->colorMode]; ++i) {
#ifdef SPLASH_CMYK
          if (imgData->colorMode == splashModeCMYK8 || imgData->colorMode == splashModeDeviceN8)
            dest[i] = div255(pat[i] * (255 - col[0]));
          else
#endif
            dest[i] = 255 - div255((255 - pat[i]) * (255 - col[0]));
        }
        dest += splashColorModeNComps[imgData->colorMode];
      }
    }
    if (alphaLine != nullptr) {
      const int y = (imgData->y == imgData->bitmap->getHeight() - 1 && imgData->y > 50) ? imgData->y - 1 : imgData->y;
      SplashColorPtr aq = alphaLine;
      SplashColorPtr p;
      const int n = imgData->bitmap->getWidth();
      for (int m = 0; m < imgData->repeatX; m++) {
        p = imgData->bitmap->getAlphaPtr() + y * imgData->bitmap->getWidth();
        for (int x = 0; x < n; ++x) {
          *aq++ = *p++;
        }
      }
    }
  }
  ++imgData->y;
  return true;
}