void Magick::Image::channelDepth(const ChannelType channel_,
  const size_t depth_)
{
  modifyImage();
  GetPPException;
  GetAndSetPPChannelMask(channel_);
  SetImageDepth(image(),depth_,exceptionInfo);
  RestorePPChannelMask;
  ThrowImageException;
}