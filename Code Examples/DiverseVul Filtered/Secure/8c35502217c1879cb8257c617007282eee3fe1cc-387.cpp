void Magick::Image::brightnessContrastChannel(const ChannelType channel_,
  const double brightness_,const double contrast_)
{
  modifyImage();
  GetPPException;
  GetAndSetPPChannelMask(channel_);
  BrightnessContrastImage(image(),brightness_,contrast_,exceptionInfo);
  RestorePPChannelMask;
  ThrowImageException;
}