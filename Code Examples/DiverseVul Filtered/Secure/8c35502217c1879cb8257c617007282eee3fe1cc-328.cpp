void Magick::Image::adaptiveSharpen(const double radius_,const double sigma_)
{
  MagickCore::Image
    *newImage;

  GetPPException;
  newImage=AdaptiveSharpenImage(constImage(),radius_,sigma_,exceptionInfo);
  replaceImage(newImage);
  ThrowImageException;
}