bool SplashAxialPattern::getParameter(double xc, double yc, double *t) {
  double s;

  xc -= x0;
  yc -= y0;

  s = (xc * dx + yc * dy) * mul;
  if (0 <= s && s <= 1) {
    *t = t0 + dt * s;
  } else if (s < 0 && shading->getExtend0()) {
    *t = t0;
  } else if (s > 1 && shading->getExtend1()) {
    *t = t1;
  } else {
    return false;
  }

  return true;
}