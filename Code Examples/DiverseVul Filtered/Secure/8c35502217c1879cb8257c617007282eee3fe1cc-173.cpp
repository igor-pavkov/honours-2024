void Magick::Image::levelizeChannel(const ChannelType channel_,
  const double blackPoint_,const double whitePoint_,const double gamma_)
{
  modifyImage();
  GetPPException;
  GetAndSetPPChannelMask(channel_);
  (void) LevelizeImage(image(),blackPoint_,whitePoint_,gamma_,exceptionInfo);
  RestorePPChannelMask;
  ThrowImageException;
}