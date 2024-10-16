static double Blackman(const double x,
  const ResizeFilter *magick_unused(resize_filter))
{
  /*
    Blackman: 2nd order cosine windowing function:
      0.42 + 0.5 cos(pi x) + 0.08 cos(2pi x)

    Refactored by Chantal Racette and Nicolas Robidoux to one trig call and
    five flops.
  */
  const double cosine = cos((double) (MagickPI*x));
  magick_unreferenced(resize_filter);
  return(0.34+cosine*(0.5+cosine*0.16));
}