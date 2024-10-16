MagickPPExport int Magick::operator >= (const Magick::Image &left_,
  const Magick::Image &right_)
{
  return((left_ > right_) || (left_ == right_));
}