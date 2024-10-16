void Magick::Image::chromaRedPrimary(const double x_,const double y_,
  const double z_)
{
  modifyImage();
  image()->chromaticity.red_primary.x=x_;
  image()->chromaticity.red_primary.y=y_;
  image()->chromaticity.red_primary.z=z_;
}