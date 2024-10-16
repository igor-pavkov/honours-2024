void Magick::Image::trim(void)
{
  MagickCore::Image
    *newImage;

  GetPPException;
  newImage=TrimImage(constImage(),exceptionInfo);
  replaceImage(newImage);
  ThrowImageException;
}