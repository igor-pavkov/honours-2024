void Magick::Image::chromaGreenPrimary(const double x_,const double y_,
  const double z_)
{
  modifyImage();
  image()->chromaticity.green_primary.x=x_;
  image()->chromaticity.green_primary.y=y_;
  image()->chromaticity.green_primary.z=z_;
}