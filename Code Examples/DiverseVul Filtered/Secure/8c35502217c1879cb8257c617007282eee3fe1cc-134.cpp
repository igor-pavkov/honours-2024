void Magick::Image::gamma(const double gamma_)
{
  modifyImage();
  GetPPException;
  GammaImage(image(),gamma_,exceptionInfo);
  ThrowImageException;
}