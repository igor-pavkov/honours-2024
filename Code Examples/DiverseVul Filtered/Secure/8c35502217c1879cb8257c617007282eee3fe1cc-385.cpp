void Magick::Image::selectiveBlurChannel(const ChannelType channel_,
  const double radius_,const double sigma_,const double threshold_)
{
  MagickCore::Image
    *newImage;

  GetPPException;
  GetAndSetPPChannelMask(channel_);
  newImage=SelectiveBlurImage(constImage(),radius_,sigma_,threshold_,
    exceptionInfo);
  RestorePPChannelMask;
  replaceImage(newImage);
  ThrowImageException;
}