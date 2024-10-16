void Magick::Image::kuwahara(const double radius_,const double sigma_)
{
  MagickCore::Image
    *newImage;

  GetPPException;
  newImage=KuwaharaImage(constImage(),radius_,sigma_,exceptionInfo);
  replaceImage(newImage);
  ThrowImageException;
}