void Magick::Image::edge(const double radius_)
{
  MagickCore::Image
    *newImage;

  GetPPException;
  newImage=EdgeImage(constImage(),radius_,exceptionInfo);
  replaceImage(newImage);
  ThrowImageException;
}