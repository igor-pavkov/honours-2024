void Magick::Image::autoGamma(void)
{
  modifyImage();
  GetPPException;
  (void) SyncImageSettings(imageInfo(),image(),exceptionInfo);
  (void) AutoGammaImage(image(),exceptionInfo);
  ThrowImageException;
}