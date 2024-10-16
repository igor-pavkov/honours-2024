void Magick::Image::minify(void)
{
  MagickCore::Image
    *newImage;

  GetPPException;
  newImage=MinifyImage(constImage(),exceptionInfo);
  replaceImage(newImage);
  ThrowImageException;
}