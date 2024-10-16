void Magick::Image::whiteThresholdChannel(const ChannelType channel_,
  const std::string &threshold_)
{
  modifyImage();
  GetPPException;
  GetAndSetPPChannelMask(channel_);
  WhiteThresholdImage(image(),threshold_.c_str(),exceptionInfo);
  RestorePPChannelMask;
  ThrowImageException;
}