static double Triangle(const double x,
  const ResizeFilter *magick_unused(resize_filter))
{
  magick_unreferenced(resize_filter);

  /*
    1st order (linear) B-Spline, bilinear interpolation, Tent 1D filter, or
    a Bartlett 2D Cone filter.  Also used as a Bartlett Windowing function
    for Sinc().
  */
  if (x < 1.0)
    return(1.0-x);
  return(0.0);
}