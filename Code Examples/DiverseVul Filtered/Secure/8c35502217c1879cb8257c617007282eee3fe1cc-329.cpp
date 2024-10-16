MagickPPExport int Magick::operator < (const Magick::Image &left_,
  const Magick::Image &right_)
{
  // If image pixels are less, then image is smaller
  return((left_.rows() * left_.columns()) <
    (right_.rows() * right_.columns()));
}