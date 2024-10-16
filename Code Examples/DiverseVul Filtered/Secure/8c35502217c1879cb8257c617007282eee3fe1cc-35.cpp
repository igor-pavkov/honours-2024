void Magick::Image::blueShift(const double factor_)
{
  MagickCore::Image
    *newImage;

  GetPPException;
  newImage=BlueShiftImage(constImage(),factor_,exceptionInfo);
  replaceImage(newImage);
  ThrowImageException;
}