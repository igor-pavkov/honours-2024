bool SplashOutputDev::radialShadedFill(GfxState *state, GfxRadialShading *shading, double tMin, double tMax) {
  SplashRadialPattern *pattern = new SplashRadialPattern(colorMode, state, shading);
  bool retVal = univariateShadedFill(state, pattern, tMin, tMax);

  delete pattern;

  return retVal;
}