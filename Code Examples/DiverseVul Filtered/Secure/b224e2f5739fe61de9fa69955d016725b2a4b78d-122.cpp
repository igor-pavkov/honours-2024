void SplashOutputDev::drawSoftMaskedImage(GfxState *state, Object * /* ref */,
					  Stream *str, int width, int height,
					  GfxImageColorMap *colorMap,
					  bool interpolate,
					  Stream *maskStr,
					  int maskWidth, int maskHeight,
					  GfxImageColorMap *maskColorMap,
					  bool maskInterpolate) {
  SplashCoord mat[6];
  SplashOutImageData imgData;
  SplashOutImageData imgMaskData;
  SplashColorMode srcMode;
  SplashBitmap *maskBitmap;
  Splash *maskSplash;
  SplashColor maskColor;
  GfxGray gray;
  GfxRGB rgb;
#ifdef SPLASH_CMYK
  GfxCMYK cmyk;
  GfxColor deviceN;
#endif
  unsigned char pix;

#ifdef SPLASH_CMYK
  colorMap->getColorSpace()->createMapping(bitmap->getSeparationList(), SPOT_NCOMPS);
#endif
  setOverprintMask(colorMap->getColorSpace(), state->getFillOverprint(),
		   state->getOverprintMode(), nullptr);

  const double *ctm = state->getCTM();
  for (int i = 0; i < 6; ++i) {
    if (!std::isfinite(ctm[i])) return;
  }
  mat[0] = ctm[0];
  mat[1] = ctm[1];
  mat[2] = -ctm[2];
  mat[3] = -ctm[3];
  mat[4] = ctm[2] + ctm[4];
  mat[5] = ctm[3] + ctm[5];

  //----- set up the soft mask

  if (maskColorMap->getMatteColor() != nullptr) {
    int maskChars;
    if (checkedMultiply(maskWidth, maskHeight, &maskChars)) {
      return;
    }
    unsigned char *data = (unsigned char *) gmalloc(maskChars);
    maskStr->reset();
    const int readChars = maskStr->doGetChars(maskChars, data);
    if (unlikely(readChars < maskChars)) {
      memset(&data[readChars], 0, maskChars - readChars);
    }
    maskStr->close();
    maskStr = new AutoFreeMemStream((char *)data, 0, maskChars, maskStr->getDictObject()->copy());
  }
  imgMaskData.imgStr = new ImageStream(maskStr, maskWidth,
				       maskColorMap->getNumPixelComps(),
				       maskColorMap->getBits());
  imgMaskData.imgStr->reset();
  imgMaskData.colorMap = maskColorMap;
  imgMaskData.maskColors = nullptr;
  imgMaskData.colorMode = splashModeMono8;
  imgMaskData.width = maskWidth;
  imgMaskData.height = maskHeight;
  imgMaskData.y = 0;
  imgMaskData.maskStr = nullptr;
  imgMaskData.maskColorMap = nullptr;
  const unsigned imgMaskDataLookupSize = 1 << maskColorMap->getBits();
  imgMaskData.lookup = (SplashColorPtr)gmalloc(imgMaskDataLookupSize);
  for (unsigned i = 0; i < imgMaskDataLookupSize; ++i) {
    pix = (unsigned char)i;
    maskColorMap->getGray(&pix, &gray);
    imgMaskData.lookup[i] = colToByte(gray);
  }
  maskBitmap = new SplashBitmap(bitmap->getWidth(), bitmap->getHeight(),
				1, splashModeMono8, false);
  maskSplash = new Splash(maskBitmap, vectorAntialias);
  maskColor[0] = 0;
  maskSplash->clear(maskColor);
  maskSplash->drawImage(&imageSrc, nullptr, &imgMaskData, splashModeMono8, false,
			maskWidth, maskHeight, mat, maskInterpolate);
  delete imgMaskData.imgStr;
  if (maskColorMap->getMatteColor() == nullptr) {
    maskStr->close();
  }
  gfree(imgMaskData.lookup);
  delete maskSplash;
  splash->setSoftMask(maskBitmap);

  //----- draw the source image

  imgData.imgStr = new ImageStream(str, width,
				   colorMap->getNumPixelComps(),
				   colorMap->getBits());
  imgData.imgStr->reset();
  imgData.colorMap = colorMap;
  imgData.maskColors = nullptr;
  imgData.colorMode = colorMode;
  imgData.width = width;
  imgData.height = height;
  imgData.maskStr = nullptr;
  imgData.maskColorMap = nullptr;
  if (maskColorMap->getMatteColor() != nullptr) {
    getMatteColor(colorMode, colorMap, maskColorMap->getMatteColor(), imgData.matteColor);
    imgData.maskColorMap = maskColorMap;
    imgData.maskStr = new ImageStream(maskStr, maskWidth,
				       maskColorMap->getNumPixelComps(),
				       maskColorMap->getBits());
    imgData.maskStr->reset();
  }
  imgData.y = 0;

  // special case for one-channel (monochrome/gray/separation) images:
  // build a lookup table here
  imgData.lookup = nullptr;
  if (colorMap->getNumPixelComps() == 1) {
    const unsigned n = 1 << colorMap->getBits();
    switch (colorMode) {
    case splashModeMono1:
    case splashModeMono8:
      imgData.lookup = (SplashColorPtr)gmalloc(n);
      for (unsigned i = 0; i < n; ++i) {
	pix = (unsigned char)i;
	colorMap->getGray(&pix, &gray);
	imgData.lookup[i] = colToByte(gray);
      }
      break;
    case splashModeRGB8:
    case splashModeBGR8:
      imgData.lookup = (SplashColorPtr)gmallocn_checkoverflow(n, 3);
      if (likely(imgData.lookup != nullptr)) {
	for (unsigned i = 0; i < n; ++i) {
	  pix = (unsigned char)i;
	  colorMap->getRGB(&pix, &rgb);
	  imgData.lookup[3*i] = colToByte(rgb.r);
	  imgData.lookup[3*i+1] = colToByte(rgb.g);
	  imgData.lookup[3*i+2] = colToByte(rgb.b);
	}
      }
      break;
    case splashModeXBGR8:
      imgData.lookup = (SplashColorPtr)gmallocn_checkoverflow(n, 4);
      if (likely(imgData.lookup != nullptr)) {
	for (unsigned i = 0; i < n; ++i) {
	  pix = (unsigned char)i;
	  colorMap->getRGB(&pix, &rgb);
	  imgData.lookup[4*i] = colToByte(rgb.r);
	  imgData.lookup[4*i+1] = colToByte(rgb.g);
	  imgData.lookup[4*i+2] = colToByte(rgb.b);
	  imgData.lookup[4*i+3] = 255;
	}
      }
      break;
#ifdef SPLASH_CMYK
    case splashModeCMYK8:
      imgData.lookup = (SplashColorPtr)gmallocn_checkoverflow(n, 4);
      if (likely(imgData.lookup != nullptr)) {
	for (unsigned i = 0; i < n; ++i) {
	  pix = (unsigned char)i;
	  colorMap->getCMYK(&pix, &cmyk);
	  imgData.lookup[4*i] = colToByte(cmyk.c);
	  imgData.lookup[4*i+1] = colToByte(cmyk.m);
	  imgData.lookup[4*i+2] = colToByte(cmyk.y);
	  imgData.lookup[4*i+3] = colToByte(cmyk.k);
	}
      }
      break;
    case splashModeDeviceN8:
      imgData.lookup = (SplashColorPtr)gmallocn_checkoverflow(n, SPOT_NCOMPS+4);
      if (likely(imgData.lookup != nullptr)) {
	for (unsigned i = 0; i < n; ++i) {
	  pix = (unsigned char)i;
	  colorMap->getDeviceN(&pix, &deviceN);
	  for (int cp = 0; cp < SPOT_NCOMPS+4; cp++)
	    imgData.lookup[(SPOT_NCOMPS+4)*i + cp] = colToByte(deviceN.c[cp]);
	}
      }
      break;
#endif
    }
  }

  if (colorMode == splashModeMono1) {
    srcMode = splashModeMono8;
  } else {
    srcMode = colorMode;
  }
  splash->drawImage(&imageSrc, nullptr, &imgData, srcMode, false, width, height, mat, interpolate);
  splash->setSoftMask(nullptr);
  gfree(imgData.lookup);
  delete imgData.maskStr;
  delete imgData.imgStr;
  if (maskColorMap->getMatteColor() != nullptr) {
    maskStr->close();
    delete maskStr;
  }
  str->close();
}