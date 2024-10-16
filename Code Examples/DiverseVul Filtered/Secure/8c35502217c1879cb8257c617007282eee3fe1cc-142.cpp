void Magick::Image::emboss(const double radius_,const double sigma_)
{
  MagickCore::Image
    *newImage;

  GetPPException;
  newImage=EmbossImage(constImage(),radius_,sigma_,exceptionInfo);
  replaceImage(newImage);
  ThrowImageException;
}