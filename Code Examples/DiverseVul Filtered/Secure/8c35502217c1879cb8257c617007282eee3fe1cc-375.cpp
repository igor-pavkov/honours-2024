void Magick::Image::enhance(void)
{
  MagickCore::Image
    *newImage;

  GetPPException;
  newImage=EnhanceImage(constImage(),exceptionInfo);
  replaceImage(newImage);
  ThrowImageException;
}