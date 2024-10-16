Magick::Image Magick::Image::separate(const ChannelType channel_) const
{
  MagickCore::Image
    *image;

  GetPPException;
  image=SeparateImage(constImage(),channel_,exceptionInfo);
  ThrowImageException;
  if (image == (MagickCore::Image *) NULL)
    return(Magick::Image());
  else
    return(Magick::Image(image));
}