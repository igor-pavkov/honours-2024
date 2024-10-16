void Magick::Image::stegano(const Image &watermark_)
{
  MagickCore::Image
    *newImage;

  GetPPException;
  newImage=SteganoImage(constImage(),watermark_.constImage(),exceptionInfo);
  replaceImage(newImage);
  ThrowImageException;
}