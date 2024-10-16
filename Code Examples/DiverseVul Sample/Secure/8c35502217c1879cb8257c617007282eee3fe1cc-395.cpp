void Magick::Image::clip(void)
{
  modifyImage();
  GetPPException;
  ClipImage(image(),exceptionInfo);
  ThrowImageException;
}