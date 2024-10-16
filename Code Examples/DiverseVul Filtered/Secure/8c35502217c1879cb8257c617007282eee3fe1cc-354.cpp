void Magick::Image::chromaRedPrimary(double *x_,double *y_,double *z_) const
{
  *x_=constImage()->chromaticity.red_primary.x;
  *y_=constImage()->chromaticity.red_primary.y;
  *z_=constImage()->chromaticity.red_primary.z;
}