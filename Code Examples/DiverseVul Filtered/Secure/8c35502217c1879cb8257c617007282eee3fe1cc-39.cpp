void Magick::Image::vignette(const double radius_,const double sigma_,
  const ssize_t x_,const ssize_t y_)
{
  MagickCore::Image
    *newImage;

  GetPPException;
  newImage=VignetteImage(constImage(),radius_,sigma_,x_,y_,exceptionInfo);
  replaceImage(newImage);
  ThrowImageException;
}