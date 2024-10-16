void Magick::Image::levelize(const double blackPoint_,const double whitePoint_,
  const double gamma_)
{
  modifyImage();
  GetPPException;
  (void) LevelizeImage(image(),blackPoint_,whitePoint_,gamma_,exceptionInfo);
  ThrowImageException;
}