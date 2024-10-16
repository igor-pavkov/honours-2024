void Magick::Image::swirl(const double degrees_)
{
  MagickCore::Image
    *newImage;

  GetPPException;
  newImage=SwirlImage(constImage(),degrees_,image()->interpolate,
    exceptionInfo);
  replaceImage(newImage);
  ThrowImageException;
}