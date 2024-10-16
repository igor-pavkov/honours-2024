void Magick::Image::adaptiveThreshold(const size_t width_,const size_t height_,
   const double bias_)
{

  MagickCore::Image
    *newImage;

  GetPPException;
  newImage=AdaptiveThresholdImage(constImage(),width_,height_,bias_,
    exceptionInfo);
  replaceImage(newImage);
  ThrowImageException;
}