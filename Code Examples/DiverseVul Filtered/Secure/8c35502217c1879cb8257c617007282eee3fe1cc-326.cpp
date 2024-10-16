void Magick::Image::clamp(void)
{
  modifyImage();
  GetPPException;
  ClampImage(image(),exceptionInfo);
  ThrowImageException;
}