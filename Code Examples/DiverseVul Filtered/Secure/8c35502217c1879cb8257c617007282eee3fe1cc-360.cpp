void Magick::Image::rotationalBlurChannel(const ChannelType channel_,
  const double angle_)
{
  MagickCore::Image
    *newImage;

  GetPPException;
  GetAndSetPPChannelMask(channel_);
  newImage=RotationalBlurImage(constImage(),angle_,exceptionInfo);
  RestorePPChannelMask;
  replaceImage(newImage);
  ThrowImageException;
}