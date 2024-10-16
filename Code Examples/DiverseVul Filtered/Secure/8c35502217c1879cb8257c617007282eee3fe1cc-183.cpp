void Magick::Image::strip(void)
{
  modifyImage();
  GetPPException;
  StripImage(image(),exceptionInfo);
  ThrowImageException;
}