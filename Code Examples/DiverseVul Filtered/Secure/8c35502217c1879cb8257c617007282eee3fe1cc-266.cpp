void Magick::Image::channel(const ChannelType channel_)
{
  MagickCore::Image
    *newImage;

  GetPPException;
  newImage=SeparateImage(image(),channel_,exceptionInfo);
  replaceImage(newImage);
  ThrowImageException;
}