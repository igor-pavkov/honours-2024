bool SplashOutputDev::imageMaskSrc(void *data, SplashColorPtr line) {
  SplashOutImageMaskData *imgMaskData = (SplashOutImageMaskData *)data;
  unsigned char *p;
  SplashColorPtr q;
  int x;

  if (imgMaskData->y == imgMaskData->height) {
    return false;
  }
  if (!(p = imgMaskData->imgStr->getLine())) {
    return false;
  }
  for (x = 0, q = line; x < imgMaskData->width; ++x) {
    *q++ = *p++ ^ imgMaskData->invert;
  }
  ++imgMaskData->y;
  return true;
}