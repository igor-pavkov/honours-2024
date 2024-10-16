void Magick::Image::autoLevel(void)
{
  modifyImage();
  GetPPException;
  (void) AutoLevelImage(image(),exceptionInfo);
  ThrowImageException;
}