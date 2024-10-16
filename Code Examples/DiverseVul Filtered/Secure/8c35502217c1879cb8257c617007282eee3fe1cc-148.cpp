void Magick::Image::houghLine(const size_t width_,const size_t height_,
  const size_t threshold_)
{
  MagickCore::Image
    *newImage;

  GetPPException;
  newImage=HoughLineImage(constImage(),width_,height_,threshold_,
    exceptionInfo);
  replaceImage(newImage);
  ThrowImageException;
}