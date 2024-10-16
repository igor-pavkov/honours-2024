void Magick::Image::sepiaTone(const double threshold_)
{
  MagickCore::Image
    *newImage;

  GetPPException;
  newImage=SepiaToneImage(constImage(),threshold_,exceptionInfo);
  replaceImage(newImage);
  ThrowImageException;
}