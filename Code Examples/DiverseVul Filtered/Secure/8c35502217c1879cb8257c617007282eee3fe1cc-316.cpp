Magick::Image Magick::Image::mask(const PixelMask type) const
{
  MagickCore::Image
    *image;

  GetPPException;
  image = GetImageMask(constImage(),type,exceptionInfo);
  ThrowImageException;

  if (image == (MagickCore::Image *) NULL)
    return(Magick::Image());
  else
    return(Magick::Image(image));
}