void Magick::Image::unsharpmaskChannel(const ChannelType channel_,
  const double radius_,const double sigma_,const double amount_,
  const double threshold_)
{
  MagickCore::Image
    *newImage;

  GetPPException;
  GetAndSetPPChannelMask(channel_);
  newImage=UnsharpMaskImage(constImage(),radius_,sigma_,amount_,threshold_,
    exceptionInfo);
  RestorePPChannelMask;
  replaceImage(newImage);
  ThrowImageException;
}