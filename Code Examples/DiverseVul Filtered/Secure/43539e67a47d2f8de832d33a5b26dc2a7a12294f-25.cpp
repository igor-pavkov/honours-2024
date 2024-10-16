static double Bohman(const double x,
  const ResizeFilter *magick_unused(resize_filter))
{
  /*
    Bohman: 2rd Order cosine windowing function:
      (1-x) cos(pi x) + sin(pi x) / pi.

    Refactored by Nicolas Robidoux to one trig call, one sqrt call, and 7 flops,
    taking advantage of the fact that the support of Bohman is 1.0 (so that we
    know that sin(pi x) >= 0).
  */
  const double cosine = cos((double) (MagickPI*x));
  const double sine=sqrt(1.0-cosine*cosine);
  magick_unreferenced(resize_filter);
  return((1.0-x)*cosine+(1.0/MagickPI)*sine);
}