void Magick::Image::charcoal(const double radius_,const double sigma_)
{
  MagickCore::Image
    *newImage;

  GetPPException;
  newImage=CharcoalImage(image(),radius_,sigma_,exceptionInfo);
  replaceImage(newImage);
  ThrowImageException;
}