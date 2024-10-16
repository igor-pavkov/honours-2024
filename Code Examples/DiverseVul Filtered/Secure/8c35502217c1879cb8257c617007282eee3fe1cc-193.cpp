void Magick::Image::spread(const double amount_)
{
  MagickCore::Image
    *newImage;

  GetPPException;
  newImage=SpreadImage(constImage(),image()->interpolate,amount_,exceptionInfo);
  replaceImage(newImage);
  ThrowImageException;
}