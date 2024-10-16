void Magick::Image::chromaBluePrimary(double *x_,double *y_,double *z_) const
{
  *x_=constImage()->chromaticity.blue_primary.x;
  *y_=constImage()->chromaticity.blue_primary.y;
  *z_=constImage()->chromaticity.blue_primary.z;
}