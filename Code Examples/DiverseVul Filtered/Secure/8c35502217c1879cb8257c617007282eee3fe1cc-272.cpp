void Magick::Image::despeckle(void)
{
  MagickCore::Image
    *newImage;

  GetPPException;
  newImage=DespeckleImage(constImage(),exceptionInfo);
  replaceImage(newImage);
  ThrowImageException;
}