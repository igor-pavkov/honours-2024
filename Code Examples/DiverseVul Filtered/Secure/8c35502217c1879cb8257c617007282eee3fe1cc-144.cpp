void Magick::Image::fx(const std::string expression_)
{
  MagickCore::Image
    *newImage;

  GetPPException;
  newImage=FxImage(constImage(),expression_.c_str(),exceptionInfo);
  replaceImage(newImage);
  ThrowImageException;
}