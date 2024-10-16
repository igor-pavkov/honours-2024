void Magick::Image::linearStretch(const double blackPoint_,
  const double whitePoint_)
{
  modifyImage();
  GetPPException;
  LinearStretchImage(image(),blackPoint_,whitePoint_,exceptionInfo);
  ThrowImageException;
}