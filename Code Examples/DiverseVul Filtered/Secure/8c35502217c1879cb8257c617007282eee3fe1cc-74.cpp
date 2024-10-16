void Magick::Image::clut(const Image &clutImage_,
  const PixelInterpolateMethod method)
{
  modifyImage();
  GetPPException;
  ClutImage(image(),clutImage_.constImage(),method,exceptionInfo);
  ThrowImageException;
}