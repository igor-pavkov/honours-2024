void Magick::Image::unsharpmask(const double radius_,const double sigma_,
  const double amount_,const double threshold_)
{
  MagickCore::Image
    *newImage;

  GetPPException;
  newImage=UnsharpMaskImage(constImage(),radius_,sigma_,amount_,threshold_,
    exceptionInfo);
  replaceImage(newImage);
  ThrowImageException;
}