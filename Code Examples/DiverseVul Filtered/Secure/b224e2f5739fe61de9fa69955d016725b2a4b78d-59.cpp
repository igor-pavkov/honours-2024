bool SplashOutputDev::axialShadedFill(GfxState *state, GfxAxialShading *shading, double tMin, double tMax) {
  SplashAxialPattern *pattern = new SplashAxialPattern(colorMode, state, shading);
  bool retVal = univariateShadedFill(state, pattern, tMin, tMax);

  delete pattern;

  return retVal;
}