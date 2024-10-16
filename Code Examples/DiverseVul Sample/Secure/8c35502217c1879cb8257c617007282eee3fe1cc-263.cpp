Magick::Image Magick::Image::uniqueColors(void) const
{
  MagickCore::Image
    *image;

  GetPPException;
  image=UniqueImageColors(constImage(),exceptionInfo);
  ThrowImageException;
  if (image == (MagickCore::Image *) NULL)
    return(Magick::Image());
  else
    return(Magick::Image(image));
}