size_t Magick::Image::channelDepth(const ChannelType channel_)
{
  size_t
    channel_depth;

  GetPPException;
  GetAndSetPPChannelMask(channel_);
  channel_depth=GetImageDepth(constImage(),exceptionInfo);
  RestorePPChannelMask;
  ThrowImageException;
  return(channel_depth);
}