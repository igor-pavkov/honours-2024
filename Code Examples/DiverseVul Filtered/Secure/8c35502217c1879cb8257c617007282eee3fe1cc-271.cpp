void Magick::Image::level(const double blackPoint_,const double whitePoint_,
  const double gamma_)
{
  modifyImage();
  GetPPException;
  (void) LevelImage(image(),blackPoint_,whitePoint_,gamma_,exceptionInfo);
  ThrowImageException;
}