void Magick::Image::magnify(void)
{
  MagickCore::Image
    *newImage;

  GetPPException;
  newImage=MagnifyImage(constImage(),exceptionInfo);
  replaceImage(newImage);
  ThrowImageException;
}