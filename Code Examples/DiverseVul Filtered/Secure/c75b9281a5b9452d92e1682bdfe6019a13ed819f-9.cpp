static double length2d_polyline (int n, const double *x, const double *y) {
  double length = 0.0;
  for (int i = 1; i < n; i++) {
    length += sqrt((x[i] - x[i-1])*(x[i] - x[i-1])
		   +
		   (y[i] - y[i-1])*(y[i] - y[i-1]));
  }
  return length;
}