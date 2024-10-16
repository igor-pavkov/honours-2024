void Magick::Image::normalize(void)
{
  modifyImage();
  GetPPException;
  NormalizeImage(image(),exceptionInfo);
  ThrowImageException;
}