void Magick::Image::negateChannel(const ChannelType channel_,
  const bool grayscale_)
{
  modifyImage();
  GetPPException;
  GetAndSetPPChannelMask(channel_);
  NegateImage(image(),(MagickBooleanType) grayscale_,exceptionInfo);
  RestorePPChannelMask;
  ThrowImageException;
}