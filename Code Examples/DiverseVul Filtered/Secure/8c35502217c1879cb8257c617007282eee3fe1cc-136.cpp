void Magick::Image::fx(const std::string expression_,
  const Magick::ChannelType channel_)
{
  MagickCore::Image
    *newImage;

  GetPPException;
  GetAndSetPPChannelMask(channel_);
  newImage=FxImage(constImage(),expression_.c_str(),exceptionInfo);
  RestorePPChannelMask;
  replaceImage(newImage);
  ThrowImageException;
}