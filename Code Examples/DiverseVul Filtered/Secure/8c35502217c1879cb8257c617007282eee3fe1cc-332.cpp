void Magick::Image::perceptibleChannel(const ChannelType channel_,
  const double epsilon_)
{
  modifyImage();
  GetPPException;
  GetAndSetPPChannelMask(channel_);
  PerceptibleImage(image(),epsilon_,exceptionInfo);
  RestorePPChannelMask;
  ThrowImageException;
}