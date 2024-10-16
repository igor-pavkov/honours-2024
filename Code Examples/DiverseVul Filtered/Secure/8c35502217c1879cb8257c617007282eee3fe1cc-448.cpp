void Magick::Image::chromaWhitePoint(double *x_,double *y_,double *z_) const
{
  *x_=constImage()->chromaticity.white_point.x;
  *y_=constImage()->chromaticity.white_point.y;
  *z_=constImage()->chromaticity.white_point.z;
}