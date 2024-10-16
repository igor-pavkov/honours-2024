void SplashOutputDev::setOverprintMask(GfxColorSpace *colorSpace,
				       bool overprintFlag,
				       int overprintMode,
				       const GfxColor *singleColor,
				       bool grayIndexed) {
#ifdef SPLASH_CMYK
  unsigned int mask;
  GfxCMYK cmyk;
  bool additive = false;
  int i;

  if (colorSpace->getMode() == csIndexed) {
    setOverprintMask(((GfxIndexedColorSpace *)colorSpace)->getBase(),
		     overprintFlag,
		     overprintMode,
		     singleColor,
		     grayIndexed);
		return;
	}
  if (overprintFlag && overprintPreview) {
    mask = colorSpace->getOverprintMask();
    if (singleColor && overprintMode &&
	colorSpace->getMode() == csDeviceCMYK) {
      colorSpace->getCMYK(singleColor, &cmyk);
      if (cmyk.c == 0) {
	mask &= ~1;
      }
      if (cmyk.m == 0) {
	mask &= ~2;
      }
      if (cmyk.y == 0) {
	mask &= ~4;
      }
      if (cmyk.k == 0) {
	mask &= ~8;
      }
    }
    if (grayIndexed) {
      mask &= ~7;
    } else if (colorSpace->getMode() == csSeparation) {
      GfxSeparationColorSpace *deviceSep = (GfxSeparationColorSpace *)colorSpace;
      additive = deviceSep->getName()->cmp("All") != 0 && mask == 0x0f && !deviceSep->isNonMarking();
    } else if (colorSpace->getMode() == csDeviceN) {
      GfxDeviceNColorSpace *deviceNCS = (GfxDeviceNColorSpace *)colorSpace;
      additive = mask == 0x0f && !deviceNCS->isNonMarking();
      for (i = 0; i < deviceNCS->getNComps() && additive; i++) {
        if (deviceNCS->getColorantName(i)->cmp("Cyan") == 0) {
          additive = false;
        } else if (deviceNCS->getColorantName(i)->cmp("Magenta") == 0) {
          additive = false;
        } else if (deviceNCS->getColorantName(i)->cmp("Yellow") == 0) {
          additive = false;
        } else if (deviceNCS->getColorantName(i)->cmp("Black") == 0) {
          additive = false;
        }
      }
    }
  } else {
    mask = 0xffffffff;
  }
  splash->setOverprintMask(mask, additive);
#endif
}