void Magick::Image::localContrast(const double radius_,const double strength_)
{
  MagickCore::Image
    *newImage;

  GetPPException;
  newImage=LocalContrastImage(constImage(),radius_,strength_,exceptionInfo);
  replaceImage(newImage);
  ThrowImageException;
}