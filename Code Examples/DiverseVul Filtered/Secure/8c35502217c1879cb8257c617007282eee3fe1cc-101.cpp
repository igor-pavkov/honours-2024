void Magick::Image::alpha(const unsigned int alpha_)
{
  modifyImage();
  GetPPException;
  SetImageAlpha(image(),alpha_,exceptionInfo);
  ThrowImageException;
}