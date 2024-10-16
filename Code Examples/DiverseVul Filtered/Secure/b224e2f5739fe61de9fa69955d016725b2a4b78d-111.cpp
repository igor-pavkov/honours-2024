SplashUnivariatePattern::SplashUnivariatePattern(SplashColorMode colorModeA, GfxState *stateA, GfxUnivariateShading *shadingA) {
  Matrix ctm;
  double xMin, yMin, xMax, yMax;

  shading = shadingA;
  state = stateA;
  colorMode = colorModeA;

  state->getCTM(&ctm);
  ctm.invertTo(&ictm);

  // get the function domain
  t0 = shading->getDomain0();
  t1 = shading->getDomain1();
  dt = t1 - t0;

  stateA->getUserClipBBox(&xMin, &yMin, &xMax, &yMax);
  shadingA->setupCache(&ctm, xMin, yMin, xMax, yMax);
  gfxMode = shadingA->getColorSpace()->getMode();
}