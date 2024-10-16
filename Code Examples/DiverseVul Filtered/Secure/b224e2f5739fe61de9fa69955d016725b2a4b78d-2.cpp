bool SplashOutputDev::maskedImageSrc(void *data, SplashColorPtr colorLine,
				      unsigned char *alphaLine) {
  SplashOutMaskedImageData *imgData = (SplashOutMaskedImageData *)data;
  unsigned char *p, *aq;
  SplashColorPtr q, col;
  GfxRGB rgb;
  GfxGray gray;
#ifdef SPLASH_CMYK
  GfxCMYK cmyk;
  GfxColor deviceN;
#endif
  unsigned char alpha;
  unsigned char *maskPtr;
  int maskBit;
  int nComps, x;

  if (imgData->y == imgData->height) {
    return false;
  }
  if (!(p = imgData->imgStr->getLine())) {
    return false;
  }

  nComps = imgData->colorMap->getNumPixelComps();

  maskPtr = imgData->mask->getDataPtr() +
              imgData->y * imgData->mask->getRowSize();
  maskBit = 0x80;
  for (x = 0, q = colorLine, aq = alphaLine;
       x < imgData->width;
       ++x, p += nComps) {
    alpha = (*maskPtr & maskBit) ? 0xff : 0x00;
    if (!(maskBit >>= 1)) {
      ++maskPtr;
      maskBit = 0x80;
    }
    if (imgData->lookup) {
      switch (imgData->colorMode) {
      case splashModeMono1:
      case splashModeMono8:
	*q++ = imgData->lookup[*p];
	break;
      case splashModeRGB8:
      case splashModeBGR8:
	col = &imgData->lookup[3 * *p];
	*q++ = col[0];
	*q++ = col[1];
	*q++ = col[2];
	break;
      case splashModeXBGR8:
	col = &imgData->lookup[4 * *p];
	*q++ = col[0];
	*q++ = col[1];
	*q++ = col[2];
	*q++ = 255;
	break;
#ifdef SPLASH_CMYK
      case splashModeCMYK8:
	col = &imgData->lookup[4 * *p];
	*q++ = col[0];
	*q++ = col[1];
	*q++ = col[2];
	*q++ = col[3];
	break;
      case splashModeDeviceN8:
	col = &imgData->lookup[(SPOT_NCOMPS+4) * *p];
  for (int cp = 0; cp < SPOT_NCOMPS+4; cp++)
    *q++ = col[cp];
	break;
#endif
      }
      *aq++ = alpha;
    } else {
      switch (imgData->colorMode) {
      case splashModeMono1:
      case splashModeMono8:
	imgData->colorMap->getGray(p, &gray);
	*q++ = colToByte(gray);
	break;
      case splashModeXBGR8:
      case splashModeRGB8:
      case splashModeBGR8:
	imgData->colorMap->getRGB(p, &rgb);
	*q++ = colToByte(rgb.r);
	*q++ = colToByte(rgb.g);
	*q++ = colToByte(rgb.b);
	if (imgData->colorMode == splashModeXBGR8) *q++ = 255;
	break;
#ifdef SPLASH_CMYK
      case splashModeCMYK8:
	imgData->colorMap->getCMYK(p, &cmyk);
	*q++ = colToByte(cmyk.c);
	*q++ = colToByte(cmyk.m);
	*q++ = colToByte(cmyk.y);
	*q++ = colToByte(cmyk.k);
	break;
      case splashModeDeviceN8:
	imgData->colorMap->getDeviceN(p, &deviceN);
  for (int cp = 0; cp < SPOT_NCOMPS+4; cp++)
    *q++ = colToByte(deviceN.c[cp]);
	break;
#endif
      }
      *aq++ = alpha;
    }
  }

  ++imgData->y;
  return true;
}