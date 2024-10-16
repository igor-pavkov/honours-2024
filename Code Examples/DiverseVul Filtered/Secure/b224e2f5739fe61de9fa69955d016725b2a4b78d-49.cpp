void SplashOutputDev::iccTransform(void *data, SplashBitmap *bitmap) {
  SplashOutImageData *imgData = (SplashOutImageData *)data;
  int nComps = imgData->colorMap->getNumPixelComps();

  unsigned char *colorLine = (unsigned char *) gmalloc(nComps * bitmap->getWidth());
  unsigned char *rgbxLine = (imgData->colorMode == splashModeXBGR8) ? (unsigned char *) gmalloc(3 * bitmap->getWidth()) : nullptr;
  for (int i = 0; i < bitmap->getHeight(); i++) {
    unsigned char *p = bitmap->getDataPtr() + i * bitmap->getRowSize();
    switch (imgData->colorMode) {
    case splashModeMono1:
    case splashModeMono8:
      imgData->colorMap->getGrayLine(p, colorLine, bitmap->getWidth());
      memcpy(p, colorLine, nComps * bitmap->getWidth());
      break;
    case splashModeRGB8:
    case splashModeBGR8:
      imgData->colorMap->getRGBLine(p, colorLine, bitmap->getWidth());
      memcpy(p, colorLine, nComps * bitmap->getWidth());
      break;
#ifdef SPLASH_CMYK
    case splashModeCMYK8:
      imgData->colorMap->getCMYKLine(p, colorLine, bitmap->getWidth());
      memcpy(p, colorLine, nComps * bitmap->getWidth());
      break;
    case splashModeDeviceN8:
      imgData->colorMap->getDeviceNLine(p, colorLine, bitmap->getWidth());
      memcpy(p, colorLine, nComps * bitmap->getWidth());
      break;
#endif
    case splashModeXBGR8:
      unsigned char *q;
      unsigned char *b = p;
      int x;
      for (x = 0, q = rgbxLine; x < bitmap->getWidth(); ++x, b+=4) {
        *q++ = b[2];
        *q++ = b[1];
        *q++ = b[0];
      }
      imgData->colorMap->getRGBLine(rgbxLine, colorLine, bitmap->getWidth());
      b = p;
      for (x = 0, q = colorLine; x < bitmap->getWidth(); ++x, b+=4) {
        b[2] = *q++;
        b[1] = *q++;
        b[0] = *q++;
      }
      break;
    }
  }
  gfree(colorLine);
  if (rgbxLine != nullptr) 
    gfree(rgbxLine);
}