static double Sinc(const double x,
  const ResizeFilter *magick_unused(resize_filter))
{
  magick_unreferenced(resize_filter);

  /*
    Scaled sinc(x) function using a trig call:
      sinc(x) == sin(pi x)/(pi x).
  */
  if (x != 0.0)
    {
      const double alpha=(double) (MagickPI*x);
      return(sin((double) alpha)/alpha);
    }
  return((double) 1.0);
}