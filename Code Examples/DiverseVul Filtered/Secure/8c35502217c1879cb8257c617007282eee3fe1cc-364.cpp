void Magick::Image::shadow(const double percent_opacity_,const double sigma_,
  const ssize_t x_,const ssize_t y_)
{
  MagickCore::Image
    *newImage;

  GetPPException;
  newImage=ShadowImage(constImage(),percent_opacity_, sigma_,x_, y_,
    exceptionInfo);
  replaceImage(newImage);
  ThrowImageException;
}