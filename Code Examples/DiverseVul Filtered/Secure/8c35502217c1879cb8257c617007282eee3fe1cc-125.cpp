void Magick::Image::selectiveBlur(const double radius_,const double sigma_,
  const double threshold_)
{
  MagickCore::Image
    *newImage;

  GetPPException;
  newImage=SelectiveBlurImage(constImage(),radius_,sigma_,threshold_,
    exceptionInfo);
  replaceImage(newImage);
  ThrowImageException;
}