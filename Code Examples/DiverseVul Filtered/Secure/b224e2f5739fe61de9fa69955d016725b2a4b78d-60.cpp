SplashRadialPattern::SplashRadialPattern(SplashColorMode colorModeA, GfxState *stateA, GfxRadialShading *shadingA):
  SplashUnivariatePattern(colorModeA, stateA, shadingA)
{
  SplashColor defaultColor;
  GfxColor srcColor;

  shadingA->getCoords(&x0, &y0, &r0, &dx, &dy, &dr);
  dx -= x0;
  dy -= y0;
  dr -= r0;
  a = dx*dx + dy*dy - dr*dr;
  if (fabs(a) > RADIAL_EPSILON)
    inva = 1.0 / a;
  shadingA->getColorSpace()->getDefaultColor(&srcColor);
  convertGfxColor(defaultColor, colorModeA, shadingA->getColorSpace(), &srcColor);
}