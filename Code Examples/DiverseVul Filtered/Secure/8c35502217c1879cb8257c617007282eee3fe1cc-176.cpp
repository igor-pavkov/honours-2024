void Magick::Image::polaroid(const std::string &caption_,const double angle_,
  const PixelInterpolateMethod method_)
{
  MagickCore::Image
    *newImage;

  GetPPException;
  newImage=PolaroidImage(constImage(),options()->drawInfo(),caption_.c_str(),
    angle_,method_,exceptionInfo);
  replaceImage(newImage);
  ThrowImageException;
}