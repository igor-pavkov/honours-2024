void Magick::Image::randomThresholdChannel(const ChannelType channel_,
  const double low_,const double high_)
{
  modifyImage();
  GetPPException;
  GetAndSetPPChannelMask(channel_);
  (void) RandomThresholdImage(image(),low_,high_,exceptionInfo);
  RestorePPChannelMask;
  ThrowImageException;
}