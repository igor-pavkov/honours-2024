void Magick::Image::rotationalBlur(const double angle_)
{
  MagickCore::Image
    *newImage;

  GetPPException;
  newImage=RotationalBlurImage(constImage(),angle_,exceptionInfo);
  replaceImage(newImage);
  ThrowImageException;
}