SplashFunctionPattern::SplashFunctionPattern(SplashColorMode colorModeA, GfxState *stateA, GfxFunctionShading *shadingA)
{
  Matrix ctm;
  SplashColor defaultColor;
  GfxColor srcColor;
  const double *matrix = shadingA->getMatrix();

  shading = shadingA;
  state = stateA;
  colorMode = colorModeA;

  state->getCTM(&ctm);

  double a1 = ctm.m[0];
  double b1 = ctm.m[1];
  double c1 = ctm.m[2];
  double d1 = ctm.m[3];

  ctm.m[0] = matrix[0] * a1 + matrix[1] * c1;
  ctm.m[1] = matrix[0] * b1 + matrix[1] * d1;
  ctm.m[2] = matrix[2] * a1 + matrix[3] * c1;
  ctm.m[3] = matrix[2] * b1 + matrix[3] * d1;
  ctm.m[4] = matrix[4] * a1 + matrix[5] * c1 + ctm.m[4];
  ctm.m[5] = matrix[4] * b1 + matrix[5] * d1 + ctm.m[5];
  ctm.invertTo(&ictm);

  gfxMode = shadingA->getColorSpace()->getMode();
  shadingA->getColorSpace()->getDefaultColor(&srcColor);
  shadingA->getDomain(&xMin, &yMin, &xMax, &yMax);
  convertGfxColor(defaultColor, colorModeA, shadingA->getColorSpace(), &srcColor);
}