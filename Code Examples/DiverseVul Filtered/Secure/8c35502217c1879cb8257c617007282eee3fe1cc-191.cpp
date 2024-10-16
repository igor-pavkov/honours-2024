void Magick::Image::syncPixels(void)
{
  GetPPException;
  (void) SyncAuthenticPixels(image(),exceptionInfo);
  ThrowImageException;
}