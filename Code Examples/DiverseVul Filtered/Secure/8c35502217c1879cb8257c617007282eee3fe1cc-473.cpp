void Magick::Image::flop(void)
{
  MagickCore::Image
    *newImage;

  GetPPException;
  newImage=FlopImage(constImage(),exceptionInfo);
  replaceImage(newImage);
  ThrowImageException;
}