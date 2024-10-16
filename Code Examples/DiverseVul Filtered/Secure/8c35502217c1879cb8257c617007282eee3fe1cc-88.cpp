void  Magick::Image::haldClut(const Image &clutImage_)
{
  modifyImage();
  GetPPException;
  (void) HaldClutImage(image(),clutImage_.constImage(),exceptionInfo);
  ThrowImageException;
}