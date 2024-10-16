bool SplashOutputDev::gouraudTriangleShadedFill(GfxState *state, GfxGouraudTriangleShading *shading)
{
  GfxColorSpaceMode shadingMode = shading->getColorSpace()->getMode();
  bool bDirectColorTranslation = false; // triggers an optimization.
  switch (colorMode) {
    case splashModeRGB8:
      bDirectColorTranslation = (shadingMode == csDeviceRGB);
    break;
#ifdef SPLASH_CMYK
    case splashModeCMYK8:
    case splashModeDeviceN8:
      bDirectColorTranslation = (shadingMode == csDeviceCMYK);
    break;
#endif
    default:
    break;
  }
  // restore vector antialias because we support it here
  if (shading->isParameterized()) {
    SplashGouraudColor *splashShading = new SplashGouraudPattern(bDirectColorTranslation, state, shading);
    bool vaa = getVectorAntialias();
    bool retVal = false;
    setVectorAntialias(true);
    retVal = splash->gouraudTriangleShadedFill(splashShading);
    setVectorAntialias(vaa);
    delete splashShading;
    return retVal;
  }
  return false;
}