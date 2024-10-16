static double Box(const double magick_unused(x),
  const ResizeFilter *magick_unused(resize_filter))
{
  magick_unreferenced(x);
  magick_unreferenced(resize_filter);

  /*
    A Box filter is a equal weighting function (all weights equal).
    DO NOT LIMIT results by support or resize point sampling will work
    as it requests points beyond its normal 0.0 support size.
  */
  return(1.0);
}