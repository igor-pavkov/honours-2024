void Magick::Image::chromaWhitePoint(const double x_,const double y_,
  const double z_)
{
  modifyImage();
  image()->chromaticity.white_point.x=x_;
  image()->chromaticity.white_point.y=y_;
  image()->chromaticity.white_point.z=z_;
}