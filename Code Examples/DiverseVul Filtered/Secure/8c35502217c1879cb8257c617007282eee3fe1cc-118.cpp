void Magick::Image::clampChannel(const ChannelType channel_)
{
  modifyImage();
  GetPPException;
  GetAndSetPPChannelMask(channel_);
  ClampImage(image(),exceptionInfo);
  RestorePPChannelMask;
  ThrowImageException;
}