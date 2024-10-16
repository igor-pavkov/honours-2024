bool SplashFunctionPattern::getColor(int x, int y, SplashColorPtr c) {
  GfxColor gfxColor;
  double xc, yc;

  ictm.transform(x, y, &xc, &yc);
  if (xc < xMin || xc > xMax || yc < yMin || yc > yMax) return false;
  shading->getColor(xc, yc, &gfxColor);
  convertGfxColor(c, colorMode, shading->getColorSpace(), &gfxColor);
  return true;
}