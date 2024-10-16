void Magick::Image::contrastStretchChannel(const ChannelType channel_,
  const double blackPoint_,const double whitePoint_)
{
  modifyImage();
  GetPPException;
  GetAndSetPPChannelMask(channel_);
  ContrastStretchImage(image(),blackPoint_,whitePoint_,exceptionInfo);
  RestorePPChannelMask;
  ThrowImageException;
}