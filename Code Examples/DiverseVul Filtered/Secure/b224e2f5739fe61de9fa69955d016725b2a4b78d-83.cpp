void SplashOutputDev::drawImage(GfxState *state, Object *ref, Stream *str,
				int width, int height,
				GfxImageColorMap *colorMap,
				bool interpolate,
				int *maskColors, bool inlineImg) {
  SplashCoord mat[6];
  SplashOutImageData imgData;
  SplashColorMode srcMode;
  SplashImageSource src;
  SplashICCTransform tf;
  GfxGray gray;
  GfxRGB rgb;
#ifdef SPLASH_CMYK
  GfxCMYK cmyk;
  bool grayIndexed = false;
  GfxColor deviceN;
#endif
  unsigned char pix;
  int n, i;

  const double *ctm = state->getCTM();
  for (i = 0; i < 6; ++i) {
    if (!std::isfinite(ctm[i])) return;
  }
  mat[0] = ctm[0];
  mat[1] = ctm[1];
  mat[2] = -ctm[2];
  mat[3] = -ctm[3];
  mat[4] = ctm[2] + ctm[4];
  mat[5] = ctm[3] + ctm[5];

  imgData.imgStr = new ImageStream(str, width,
				   colorMap->getNumPixelComps(),
				   colorMap->getBits());
  imgData.imgStr->reset();
  imgData.colorMap = colorMap;
  imgData.maskColors = maskColors;
  imgData.colorMode = colorMode;
  imgData.width = width;
  imgData.height = height;
  imgData.maskStr = nullptr;
  imgData.maskColorMap = nullptr;
  imgData.y = 0;

  // special case for one-channel (monochrome/gray/separation) images:
  // build a lookup table here
  imgData.lookup = nullptr;
  if (colorMap->getNumPixelComps() == 1) {
    n = 1 << colorMap->getBits();
    switch (colorMode) {
    case splashModeMono1:
    case splashModeMono8:
      imgData.lookup = (SplashColorPtr)gmalloc(n);
      for (i = 0; i < n; ++i) {
	pix = (unsigned char)i;
	colorMap->getGray(&pix, &gray);
	imgData.lookup[i] = colToByte(gray);
      }
      break;
    case splashModeRGB8:
    case splashModeBGR8:
      imgData.lookup = (SplashColorPtr)gmallocn(n, 3);
      for (i = 0; i < n; ++i) {
	pix = (unsigned char)i;
	colorMap->getRGB(&pix, &rgb);
	imgData.lookup[3*i] = colToByte(rgb.r);
	imgData.lookup[3*i+1] = colToByte(rgb.g);
	imgData.lookup[3*i+2] = colToByte(rgb.b);
      }
      break;
    case splashModeXBGR8:
      imgData.lookup = (SplashColorPtr)gmallocn_checkoverflow(n, 4);
      if (likely(imgData.lookup != nullptr)) {
	for (i = 0; i < n; ++i) {
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
      grayIndexed = colorMap->getColorSpace()->getMode() != csDeviceGray;
      imgData.lookup = (SplashColorPtr)gmallocn(n, 4);
      for (i = 0; i < n; ++i) {
	pix = (unsigned char)i;
	colorMap->getCMYK(&pix, &cmyk);
	if (cmyk.c != 0 || cmyk.m != 0 || cmyk.y != 0) {
	  grayIndexed = false;
	}
	imgData.lookup[4*i] = colToByte(cmyk.c);
	imgData.lookup[4*i+1] = colToByte(cmyk.m);
	imgData.lookup[4*i+2] = colToByte(cmyk.y);
	imgData.lookup[4*i+3] = colToByte(cmyk.k);
      }
      break;
    case splashModeDeviceN8:
      colorMap->getColorSpace()->createMapping(bitmap->getSeparationList(), SPOT_NCOMPS);
      grayIndexed = colorMap->getColorSpace()->getMode() != csDeviceGray;
      imgData.lookup = (SplashColorPtr)gmallocn(n, SPOT_NCOMPS+4);
      for (i = 0; i < n; ++i) {
        pix = (unsigned char)i;
        colorMap->getCMYK(&pix, &cmyk);
        if (cmyk.c != 0 || cmyk.m != 0 || cmyk.y != 0) {
          grayIndexed = false;
        }
        colorMap->getDeviceN(&pix, &deviceN);
        for (int cp = 0; cp < SPOT_NCOMPS+4; cp++)
          imgData.lookup[(SPOT_NCOMPS+4)*i +cp] = colToByte(deviceN.c[cp]);
      }
      break;
#endif
    }
  }

#ifdef SPLASH_CMYK
  setOverprintMask(colorMap->getColorSpace(), state->getFillOverprint(),
		   state->getOverprintMode(), nullptr, grayIndexed);
#else		   
  setOverprintMask(colorMap->getColorSpace(), state->getFillOverprint(),
		   state->getOverprintMode(), nullptr);
#endif		   

  if (colorMode == splashModeMono1) {
    srcMode = splashModeMono8;
  } else {
    srcMode = colorMode;
  }
#ifdef USE_CMS
  src = maskColors ? &alphaImageSrc : useIccImageSrc(&imgData) ? &iccImageSrc : &imageSrc;
  tf = maskColors == nullptr && useIccImageSrc(&imgData) ? &iccTransform : nullptr;
#else
  src = maskColors ? &alphaImageSrc : &imageSrc;
  tf = nullptr;
#endif
  splash->drawImage(src, tf, &imgData, srcMode, maskColors ? true : false,
		    width, height, mat, interpolate);
  if (inlineImg) {
    while (imgData.y < height) {
      imgData.imgStr->getLine();
      ++imgData.y;
    }
  }

  gfree(imgData.lookup);
  delete imgData.imgStr;
  str->close();
}