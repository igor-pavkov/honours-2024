bool SplashUnivariatePattern::testPosition(int x, int y) {
  double xc, yc, t;

  ictm.transform(x, y, &xc, &yc);
  if (! getParameter (xc, yc, &t))
      return false;
  return (t0 < t1) ? (t > t0 && t < t1) : (t > t1 && t < t0);
}