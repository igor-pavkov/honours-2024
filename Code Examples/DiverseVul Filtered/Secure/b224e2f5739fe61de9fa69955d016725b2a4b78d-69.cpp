bool SplashOutputDev::iccImageSrc(void *data, SplashColorPtr colorLine,
				unsigned char * /*alphaLine*/) {
  SplashOutImageData *imgData = (SplashOutImageData *)data;
  unsigned char *p;
  int nComps;

  if (imgData->y == imgData->height) {
    return false;
  }
  if (!(p = imgData->imgStr->getLine())) {
    int destComps = 1;
    if (imgData->colorMode == splashModeRGB8 || imgData->colorMode == splashModeBGR8)
        destComps = 3;
    else if (imgData->colorMode == splashModeXBGR8)
        destComps = 4;
#ifdef SPLASH_CMYK
    else if (imgData->colorMode == splashModeCMYK8)
        destComps = 4;
    else if (imgData->colorMode == splashModeDeviceN8)
        destComps = SPOT_NCOMPS + 4;
#endif
    memset(colorLine, 0, imgData->width * destComps);
    return false;
  }

  if (imgData->colorMode == splashModeXBGR8) {
    SplashColorPtr q;
    int x;
    for (x = 0, q = colorLine; x < imgData->width; ++x) {
      *q++ = *p++;
      *q++ = *p++;
      *q++ = *p++;
      *q++ = 255;
    }
  } else {
    nComps = imgData->colorMap->getNumPixelComps();
    memcpy(colorLine, p, imgData->width * nComps);
  }

  ++imgData->y;
  return true;
}