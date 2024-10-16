void Magick::Image::erase(void)
{
  modifyImage();
  GetPPException;
  (void) SetImageBackgroundColor(image(),exceptionInfo);
  ThrowImageException;
}