static double Jinc(const double x,
  const ResizeFilter *magick_unused(resize_filter))
{
  magick_unreferenced(resize_filter);

  /*
    See Pratt "Digital Image Processing" p.97 for Jinc/Bessel functions.
    http://mathworld.wolfram.com/JincFunction.html and page 11 of
    http://www.ph.ed.ac.uk/%7ewjh/teaching/mo/slides/lens/lens.pdf

    The original "zoom" program by Paul Heckbert called this "Bessel".  But
    really it is more accurately named "Jinc".
  */
  if (x == 0.0)
    return(0.5*MagickPI);
  return(BesselOrderOne(MagickPI*x)/x);
}