void Magick::Image::chromaGreenPrimary(double *x_,double *y_,double *z_) const
{
  *x_=constImage()->chromaticity.green_primary.x;
  *y_=constImage()->chromaticity.green_primary.y;
  *z_=constImage()->chromaticity.green_primary.z;
}