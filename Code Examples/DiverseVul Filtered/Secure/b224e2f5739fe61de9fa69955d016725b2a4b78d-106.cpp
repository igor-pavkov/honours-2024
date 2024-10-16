bool SplashOutputDev::useIccImageSrc(void *data) {
  SplashOutImageData *imgData = (SplashOutImageData *)data;

  if (!imgData->lookup && imgData->colorMap->getColorSpace()->getMode() == csICCBased) {
    GfxICCBasedColorSpace *colorSpace = (GfxICCBasedColorSpace *) imgData->colorMap->getColorSpace();
    switch (imgData->colorMode) {
    case splashModeMono1:
    case splashModeMono8:
      if (colorSpace->getAlt() != nullptr && colorSpace->getAlt()->getMode() == csDeviceGray)
        return true;
      break;
    case splashModeXBGR8:
    case splashModeRGB8:
    case splashModeBGR8:
      if (colorSpace->getAlt() != nullptr && colorSpace->getAlt()->getMode() == csDeviceRGB)
        return true;
      break;
#ifdef SPLASH_CMYK
    case splashModeCMYK8:
      if (colorSpace->getAlt() != nullptr && colorSpace->getAlt()->getMode() == csDeviceCMYK)
        return true;
      break;
    case splashModeDeviceN8:
      if (colorSpace->getAlt() != nullptr && colorSpace->getAlt()->getMode() == csDeviceN)
        return true;
      break;
#endif
    }
  }

  return false;
}