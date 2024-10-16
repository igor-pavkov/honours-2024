void Magick::Image::posterizeChannel(const ChannelType channel_,
  const size_t levels_,const DitherMethod method_)
{
  modifyImage();
  GetPPException;
  GetAndSetPPChannelMask(channel_);
  PosterizeImage(image(),levels_,method_,exceptionInfo);
  RestorePPChannelMask;
  ThrowImageException;
}