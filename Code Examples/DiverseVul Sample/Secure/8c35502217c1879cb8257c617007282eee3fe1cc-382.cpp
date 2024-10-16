void Magick::Image::tint(const std::string opacity_)
{
  MagickCore::Image
    *newImage;

  PixelInfo
    color;

  GetPPException;
  color=static_cast<PixelInfo>(constOptions()->fillColor());
  newImage=TintImage(constImage(),opacity_.c_str(),&color,exceptionInfo);
  replaceImage(newImage);
  ThrowImageException;
}