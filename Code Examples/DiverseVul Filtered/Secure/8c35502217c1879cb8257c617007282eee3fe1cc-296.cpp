void Magick::Image::blackPointCompensation(const bool flag_)
{
  image()->black_point_compensation=(MagickBooleanType) flag_;
}