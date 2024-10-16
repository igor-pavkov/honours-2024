void Magick::Image::sharpen(const double radius_,const double sigma_)
{
  MagickCore::Image
    *newImage;

  GetPPException;
  newImage=SharpenImage(constImage(),radius_,sigma_,exceptionInfo);
  replaceImage(newImage);
  ThrowImageException;
}