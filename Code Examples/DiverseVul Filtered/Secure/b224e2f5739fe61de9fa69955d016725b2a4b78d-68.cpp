bool SplashRadialPattern::getParameter(double xs, double ys, double *t) {
  double b, c, s0, s1;

  // We want to solve this system of equations:
  //
  // 1. (x - xc(s))^2 + (y -yc(s))^2 = rc(s)^2
  // 2. xc(s) = x0 + s * (x1 - xo)
  // 3. yc(s) = y0 + s * (y1 - yo)
  // 4. rc(s) = r0 + s * (r1 - ro)
  //
  // To simplify the system a little, we translate
  // our coordinates to have the origin in (x0,y0)

  xs -= x0;
  ys -= y0;

  // Then we have to solve the equation:
  //   A*s^2 - 2*B*s + C = 0
  // where
  //   A = dx^2  + dy^2  - dr^2
  //   B = xs*dx + ys*dy + r0*dr
  //   C = xs^2  + ys^2  - r0^2

  b = xs*dx + ys*dy + r0*dr;
  c = xs*xs + ys*ys - r0*r0;

  if (fabs(a) <= RADIAL_EPSILON) {
    // A is 0, thus the equation simplifies to:
    //   -2*B*s + C = 0
    // If B is 0, we can either have no solution or an indeterminate
    // equation, thus we behave as if we had an invalid solution
    if (fabs(b) <= RADIAL_EPSILON)
      return false;

    s0 = s1 = 0.5 * c / b;
  } else {
    double d;

    d = b*b - a*c;
    if (d < 0)
      return false;

    d = sqrt (d);
    s0 = b + d;
    s1 = b - d;

    // If A < 0, one of the two solutions will have negative radius,
    // thus it will be ignored. Otherwise we know that s1 <= s0
    // (because d >=0 implies b - d <= b + d), so if both are valid it
    // will be the true solution.
    s0 *= inva;
    s1 *= inva;
  }

  if (r0 + s0 * dr >= 0) {
    if (0 <= s0 && s0 <= 1) {
      *t = t0 + dt * s0;
      return true;
    } else if (s0 < 0 && shading->getExtend0()) {
      *t = t0;
      return true;
    } else if (s0 > 1 && shading->getExtend1()) {
      *t = t1;
      return true;
    }
  }

  if (r0 + s1 * dr >= 0) {
    if (0 <= s1 && s1 <= 1) {
      *t = t0 + dt * s1;
      return true;
    } else if (s1 < 0 && shading->getExtend0()) {
      *t = t0;
      return true;
    } else if (s1 > 1 && shading->getExtend1()) {
      *t = t1;
      return true;
    }
  }

  return false;
}