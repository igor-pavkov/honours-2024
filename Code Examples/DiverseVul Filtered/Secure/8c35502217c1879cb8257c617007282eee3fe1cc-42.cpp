void Magick::Image::levelChannel(const ChannelType channel_,
  const double blackPoint_,const double whitePoint_,const double gamma_)
{
  modifyImage();
  GetPPException;
  GetAndSetPPChannelMask(channel_);
  (void) LevelImage(image(),blackPoint_,whitePoint_,gamma_,exceptionInfo);
  RestorePPChannelMask;
  ThrowImageException;
}