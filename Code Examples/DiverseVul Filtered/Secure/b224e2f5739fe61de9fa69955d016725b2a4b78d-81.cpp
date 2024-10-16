SplashAxialPattern::SplashAxialPattern(SplashColorMode colorModeA, GfxState *stateA, GfxAxialShading *shadingA):
    SplashUnivariatePattern(colorModeA, stateA, shadingA)
{
  SplashColor defaultColor;
  GfxColor srcColor;

  shadingA->getCoords(&x0, &y0, &x1, &y1);
  dx = x1 - x0;
  dy = y1 - y0;
  const double mul_denominator = (dx * dx + dy * dy);
  if (unlikely(mul_denominator == 0)) {
    mul = 0;
  } else {
    mul = 1 / mul_denominator;
  }
  shadingA->getColorSpace()->getDefaultColor(&srcColor);
  convertGfxColor(defaultColor, colorModeA, shadingA->getColorSpace(), &srcColor);
}