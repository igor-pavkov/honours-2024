void Magick::Image::autoOrient(void)
{
  MagickCore::Image
    *newImage;

  if (image()->orientation == UndefinedOrientation ||
      image()->orientation == TopLeftOrientation)
    return;

  GetPPException;
  newImage=AutoOrientImage(constImage(),image()->orientation,exceptionInfo);
  replaceImage(newImage);
  ThrowImageException;
}