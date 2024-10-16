void Magick::Image::clutChannel(const ChannelType channel_,
  const Image &clutImage_,const PixelInterpolateMethod method)
{
  modifyImage();
  GetPPException;
  GetAndSetPPChannelMask(channel_);
  ClutImage(image(),clutImage_.constImage(),method,exceptionInfo);
  RestorePPChannelMask;
  ThrowImageException;
}