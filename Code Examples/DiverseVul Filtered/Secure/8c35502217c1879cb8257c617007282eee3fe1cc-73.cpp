bool Magick::Image::blackPointCompensation(void) const
{
  return(static_cast<bool>(constImage()->black_point_compensation));
}