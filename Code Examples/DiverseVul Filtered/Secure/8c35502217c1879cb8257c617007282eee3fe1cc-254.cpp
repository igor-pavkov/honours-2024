void Magick::Image::motionBlur(const double radius_,const double sigma_,
  const double angle_)
{
  MagickCore::Image
    *newImage;

  GetPPException;
  newImage=MotionBlurImage(constImage(),radius_,sigma_,angle_,exceptionInfo);
  replaceImage(newImage);
  ThrowImageException;
}