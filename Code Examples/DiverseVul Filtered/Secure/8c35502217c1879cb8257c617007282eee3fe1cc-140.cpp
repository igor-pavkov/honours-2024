void Magick::Image::transpose(void)
{
  MagickCore::Image
    *newImage;

  GetPPException;
  newImage=TransposeImage(constImage(),exceptionInfo);
  replaceImage(newImage);
  ThrowImageException;
}