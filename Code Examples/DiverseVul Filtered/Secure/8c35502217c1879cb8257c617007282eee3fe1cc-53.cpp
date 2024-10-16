void Magick::Image::orderedDitherChannel(const ChannelType channel_,
  std::string thresholdMap_)
{
  modifyImage();
  GetPPException;
  GetAndSetPPChannelMask(channel_);
  (void)OrderedDitherImage(image(),thresholdMap_.c_str(),exceptionInfo);
  RestorePPChannelMask;
  ThrowImageException;
}