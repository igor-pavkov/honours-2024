void Magick::Image::transverse(void)
{
  MagickCore::Image
    *newImage;

  GetPPException;
  newImage=TransverseImage(constImage(),exceptionInfo);
  replaceImage(newImage);
  ThrowImageException;
}