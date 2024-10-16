void Magick::Image::deskew(const double threshold_)
{
  MagickCore::Image
    *newImage;

  GetPPException;
  newImage=DeskewImage(constImage(),threshold_,exceptionInfo);
  replaceImage(newImage);
  ThrowImageException;
}