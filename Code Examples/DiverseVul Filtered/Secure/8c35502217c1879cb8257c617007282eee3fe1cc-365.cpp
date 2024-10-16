void Magick::Image::chromaBluePrimary(const double x_,const double y_,
  const double z_)
{
  modifyImage();
  image()->chromaticity.blue_primary.x=x_;
  image()->chromaticity.blue_primary.y=y_;
  image()->chromaticity.blue_primary.z=z_;
}