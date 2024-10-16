void Magick::Image::charcoalChannel(const ChannelType channel_,
  const double radius_,const double sigma_)
{
  MagickCore::Image
    *newImage;

  GetPPException;
  GetAndSetPPChannelMask(channel_);
  newImage=CharcoalImage(image(),radius_,sigma_,exceptionInfo);
  RestorePPChannelMask;
  replaceImage(newImage);
  ThrowImageException;
}