void Magick::Image::kuwaharaChannel(const ChannelType channel_,
  const double radius_,const double sigma_)
{
  MagickCore::Image
    *newImage;

  GetPPException;
  GetAndSetPPChannelMask(channel_);
  newImage=KuwaharaImage(constImage(),radius_,sigma_,exceptionInfo);
  replaceImage(newImage);
  RestorePPChannelMask;
  ThrowImageException;
}