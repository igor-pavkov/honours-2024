void Magick::Image::equalize(void)
{
  modifyImage();
  GetPPException;
  EqualizeImage(image(),exceptionInfo);
  ThrowImageException;
}