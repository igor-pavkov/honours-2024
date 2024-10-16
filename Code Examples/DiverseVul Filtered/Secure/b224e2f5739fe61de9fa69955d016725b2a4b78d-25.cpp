bool SplashOutputDev::imageSrc(void *data, SplashColorPtr colorLine,
				unsigned char * /*alphaLine*/) {
  SplashOutImageData *imgData = (SplashOutImageData *)data;
  unsigned char *p;
  SplashColorPtr q, col;
  GfxRGB rgb;
  GfxGray gray;
#ifdef SPLASH_CMYK
  GfxCMYK cmyk;
  GfxColor deviceN;
#endif
  int nComps, x;

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

  nComps = imgData->colorMap->getNumPixelComps();

  if (imgData->lookup) {
    switch (imgData->colorMode) {
    case splashModeMono1:
    case splashModeMono8:
      for (x = 0, q = colorLine; x < imgData->width; ++x, ++p) {
	*q++ = imgData->lookup[*p];
      }
      break;
    case splashModeRGB8:
    case splashModeBGR8:
      for (x = 0, q = colorLine; x < imgData->width; ++x, ++p) {
	col = &imgData->lookup[3 * *p];
	*q++ = col[0];
	*q++ = col[1];
	*q++ = col[2];
      }
      break;
  case splashModeXBGR8:
      for (x = 0, q = colorLine; x < imgData->width; ++x, ++p) {
	col = &imgData->lookup[4 * *p];
	*q++ = col[0];
	*q++ = col[1];
	*q++ = col[2];
	*q++ = col[3];
      }
      break;
#ifdef SPLASH_CMYK
    case splashModeCMYK8:
      for (x = 0, q = colorLine; x < imgData->width; ++x, ++p) {
	col = &imgData->lookup[4 * *p];
	*q++ = col[0];
	*q++ = col[1];
	*q++ = col[2];
	*q++ = col[3];
      }
      break;
    case splashModeDeviceN8:
      for (x = 0, q = colorLine; x < imgData->width; ++x, ++p) {
	col = &imgData->lookup[(SPOT_NCOMPS+4) * *p];
  for (int cp = 0; cp < SPOT_NCOMPS+4; cp++)
    *q++ = col[cp];
      }
      break;
#endif
    }
  } else {
    switch (imgData->colorMode) {
    case splashModeMono1:
    case splashModeMono8:
      for (x = 0, q = colorLine; x < imgData->width; ++x, p += nComps) {
	imgData->colorMap->getGray(p, &gray);
	*q++ = colToByte(gray);
      }
      break;
    case splashModeRGB8:
    case splashModeBGR8:
      if (imgData->colorMap->useRGBLine()) {
	imgData->colorMap->getRGBLine(p, (unsigned char *) colorLine, imgData->width);
      } else {
	for (x = 0, q = colorLine; x < imgData->width; ++x, p += nComps) {
	  imgData->colorMap->getRGB(p, &rgb);
	  *q++ = colToByte(rgb.r);
	  *q++ = colToByte(rgb.g);
	  *q++ = colToByte(rgb.b);
	}
      }
      break;
    case splashModeXBGR8:
      if (imgData->colorMap->useRGBLine()) {
	imgData->colorMap->getRGBXLine(p, (unsigned char *) colorLine, imgData->width);
      } else {
	for (x = 0, q = colorLine; x < imgData->width; ++x, p += nComps) {
	  imgData->colorMap->getRGB(p, &rgb);
	  *q++ = colToByte(rgb.r);
	  *q++ = colToByte(rgb.g);
	  *q++ = colToByte(rgb.b);
	  *q++ = 255;
	}
      }
      break;
#ifdef SPLASH_CMYK
    case splashModeCMYK8:
      if (imgData->colorMap->useCMYKLine()) {
	imgData->colorMap->getCMYKLine(p, (unsigned char *) colorLine, imgData->width);
      } else {
	for (x = 0, q = colorLine; x < imgData->width; ++x, p += nComps) {
	  imgData->colorMap->getCMYK(p, &cmyk);
	  *q++ = colToByte(cmyk.c);
	  *q++ = colToByte(cmyk.m);
	  *q++ = colToByte(cmyk.y);
	  *q++ = colToByte(cmyk.k);
	}
      }
      break;
    case splashModeDeviceN8:
      if (imgData->colorMap->useDeviceNLine()) {
	imgData->colorMap->getDeviceNLine(p, (unsigned char *) colorLine, imgData->width);
      } else {
        for (x = 0, q = colorLine; x < imgData->width; ++x, p += nComps) {
	  imgData->colorMap->getDeviceN(p, &deviceN);
          for (int cp = 0; cp < SPOT_NCOMPS+4; cp++)
            *q++ = colToByte(deviceN.c[cp]);
	}
      }
      break;
#endif
    }
  }

  if (imgData->maskStr != nullptr && (p = imgData->maskStr->getLine()) != nullptr) {
    int destComps = splashColorModeNComps[imgData->colorMode];
    int convComps = (imgData->colorMode == splashModeXBGR8) ? 3 : destComps;
    imgData->maskColorMap->getGrayLine(p, p, imgData->width);
    for (x = 0, q = colorLine; x < imgData->width; ++x, p++, q += destComps) {
      for (int cp = 0; cp < convComps; cp++) {
        q[cp] = (*p) ? clip255(imgData->matteColor[cp] + (int) (q[cp] - imgData->matteColor[cp]) * 255 / *p) : imgData->matteColor[cp];
      }
    }
  }
  ++imgData->y;
  return true;
}