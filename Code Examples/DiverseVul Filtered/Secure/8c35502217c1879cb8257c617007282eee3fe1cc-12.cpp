void Magick::Image::contrastStretch(const double blackPoint_,
  const double whitePoint_)
{
  modifyImage();
  GetPPException;
  ContrastStretchImage(image(),blackPoint_,whitePoint_,exceptionInfo);
  ThrowImageException;
}