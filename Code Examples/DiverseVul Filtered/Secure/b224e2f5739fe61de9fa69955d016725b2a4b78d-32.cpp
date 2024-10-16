void SplashOutputDev::drawMaskedImage(GfxState *state, Object *ref,
				      Stream *str, int width, int height,
				      GfxImageColorMap *colorMap,
				      bool interpolate,
				      Stream *maskStr, int maskWidth,
				      int maskHeight, bool maskInvert,
				      bool maskInterpolate) {
  GfxImageColorMap *maskColorMap;
  SplashCoord mat[6];
  SplashOutMaskedImageData imgData;
  SplashOutImageMaskData imgMaskData;
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
  int n, i;

#ifdef SPLASH_CMYK
  colorMap->getColorSpace()->createMapping(bitmap->getSeparationList(), SPOT_NCOMPS);
#endif
  setOverprintMask(colorMap->getColorSpace(), state->getFillOverprint(),
		   state->getOverprintMode(), nullptr);

  // If the mask is higher resolution than the image, use
  // drawSoftMaskedImage() instead.
  if (maskWidth > width || maskHeight > height) {
    Object maskDecode(new Array((xref) ? xref : doc->getXRef()));
    maskDecode.arrayAdd(Object(maskInvert ? 0 : 1));
    maskDecode.arrayAdd(Object(maskInvert ? 1 : 0));
    maskColorMap = new GfxImageColorMap(1, &maskDecode,
					new GfxDeviceGrayColorSpace());
    drawSoftMaskedImage(state, ref, str, width, height, colorMap, interpolate,
			maskStr, maskWidth, maskHeight, maskColorMap, maskInterpolate);
    delete maskColorMap;

  } else {
    //----- scale the mask image to the same size as the source image

    mat[0] = (SplashCoord)width;
    mat[1] = 0;
    mat[2] = 0;
    mat[3] = (SplashCoord)height;
    mat[4] = 0;
    mat[5] = 0;
    imgMaskData.imgStr = new ImageStream(maskStr, maskWidth, 1, 1);
    imgMaskData.imgStr->reset();
    imgMaskData.invert = maskInvert ? 0 : 1;
    imgMaskData.width = maskWidth;
    imgMaskData.height = maskHeight;
    imgMaskData.y = 0;
    maskBitmap = new SplashBitmap(width, height, 1, splashModeMono1, false);
    if (!maskBitmap->getDataPtr()) {
      delete maskBitmap;
      width = height = 1;
      maskBitmap = new SplashBitmap(width, height, 1, splashModeMono1, false);
    }
    maskSplash = new Splash(maskBitmap, false);
    maskColor[0] = 0;
    maskSplash->clear(maskColor);
    maskColor[0] = 0xff;
    maskSplash->setFillPattern(new SplashSolidColor(maskColor));
    maskSplash->fillImageMask(&imageMaskSrc, &imgMaskData,
			      maskWidth, maskHeight, mat, false);
    delete imgMaskData.imgStr;
    maskStr->close();
    delete maskSplash;

    //----- draw the source image

    const double *ctm = state->getCTM();
    for (i = 0; i < 6; ++i) {
      if (!std::isfinite(ctm[i])) {
        delete maskBitmap;
        return;
      }
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
    imgData.mask = maskBitmap;
    imgData.colorMode = colorMode;
    imgData.width = width;
    imgData.height = height;
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
	imgData.lookup = (SplashColorPtr)gmallocn(n, 4);
	for (i = 0; i < n; ++i) {
	  pix = (unsigned char)i;
	  colorMap->getRGB(&pix, &rgb);
	  imgData.lookup[4*i] = colToByte(rgb.r);
	  imgData.lookup[4*i+1] = colToByte(rgb.g);
	  imgData.lookup[4*i+2] = colToByte(rgb.b);
	  imgData.lookup[4*i+3] = 255;
	}
	break;
#ifdef SPLASH_CMYK
      case splashModeCMYK8:
	imgData.lookup = (SplashColorPtr)gmallocn(n, 4);
	for (i = 0; i < n; ++i) {
	  pix = (unsigned char)i;
	  colorMap->getCMYK(&pix, &cmyk);
	  imgData.lookup[4*i] = colToByte(cmyk.c);
	  imgData.lookup[4*i+1] = colToByte(cmyk.m);
	  imgData.lookup[4*i+2] = colToByte(cmyk.y);
	  imgData.lookup[4*i+3] = colToByte(cmyk.k);
	}
	break;
      case splashModeDeviceN8:
	imgData.lookup = (SplashColorPtr)gmallocn(n, SPOT_NCOMPS+4);
	for (i = 0; i < n; ++i) {
	  pix = (unsigned char)i;
	  colorMap->getDeviceN(&pix, &deviceN);
    for (int cp = 0; cp < SPOT_NCOMPS+4; cp++)
      imgData.lookup[(SPOT_NCOMPS+4)*i + cp] = colToByte(deviceN.c[cp]);
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
    splash->drawImage(&maskedImageSrc, nullptr, &imgData, srcMode, true,
		      width, height, mat, interpolate);
    delete maskBitmap;
    gfree(imgData.lookup);
    delete imgData.imgStr;
    str->close();
  }
}