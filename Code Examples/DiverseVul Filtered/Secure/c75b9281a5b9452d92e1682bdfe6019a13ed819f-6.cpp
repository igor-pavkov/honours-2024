static double area2d_polygon (int n, const double *x, const double *y) {
  double area = 0;
  for (int i = 1; i < n; i++) {
    area += (x[i-1] + x[i]) * (y[i] - y[i-1]);
  }
  return area / 2.0;
}