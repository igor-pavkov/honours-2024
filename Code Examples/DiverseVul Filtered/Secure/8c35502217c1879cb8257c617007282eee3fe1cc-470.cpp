void Magick::Image::posterize(const size_t levels_,const DitherMethod method_)
{
  modifyImage();
  GetPPException;
  PosterizeImage(image(),levels_,method_,exceptionInfo);
  ThrowImageException;
}