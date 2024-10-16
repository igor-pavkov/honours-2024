void Magick::Image::autoLevelChannel(const ChannelType channel_)
{
  modifyImage();
  GetPPException;
  GetAndSetPPChannelMask(channel_);
  (void) AutoLevelImage(image(),exceptionInfo);
  RestorePPChannelMask;
  ThrowImageException;
}