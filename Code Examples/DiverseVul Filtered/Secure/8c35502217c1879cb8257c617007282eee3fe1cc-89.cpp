void Magick::Image::flip(void)
{
  MagickCore::Image
    *newImage;

  GetPPException;
  newImage=FlipImage(constImage(),exceptionInfo);
  replaceImage(newImage);
  ThrowImageException;
}