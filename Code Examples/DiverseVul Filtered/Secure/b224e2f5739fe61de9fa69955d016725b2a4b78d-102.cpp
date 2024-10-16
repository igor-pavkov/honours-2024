bool SplashUnivariatePattern::getColor(int x, int y, SplashColorPtr c) {
  GfxColor gfxColor;
  double xc, yc, t;

  ictm.transform(x, y, &xc, &yc);
  if (! getParameter (xc, yc, &t))
      return false;

  const int filled = shading->getColor(t, &gfxColor);
  if (unlikely(filled < shading->getColorSpace()->getNComps())) {
    for (int i = filled; i < shading->getColorSpace()->getNComps(); ++i)
      gfxColor.c[i] = 0;
  }
  convertGfxColor(c, colorMode, shading->getColorSpace(), &gfxColor);
  return true;
}