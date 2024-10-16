void Magick::Image::implode(const double factor_)
{
  MagickCore::Image
    *newImage;

  GetPPException;
  newImage=ImplodeImage(constImage(),factor_,image()->interpolate,
    exceptionInfo);
  replaceImage(newImage);
  ThrowImageException;
}