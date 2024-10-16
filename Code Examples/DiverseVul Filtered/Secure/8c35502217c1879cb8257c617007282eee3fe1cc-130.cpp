void Magick::Image::blackThresholdChannel(const ChannelType channel_,
  const std::string &threshold_)
{
  modifyImage();
  GetPPException;
  GetAndSetPPChannelMask(channel_);
  BlackThresholdImage(image(),threshold_.c_str(),exceptionInfo);
  RestorePPChannelMask;
  ThrowImageException;
}