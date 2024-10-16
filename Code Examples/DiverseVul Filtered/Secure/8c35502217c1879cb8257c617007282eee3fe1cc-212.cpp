void Magick::Image::solarize(const double factor_)
{
  modifyImage();
  GetPPException;
  SolarizeImage(image(),factor_,exceptionInfo);
  ThrowImageException;
}