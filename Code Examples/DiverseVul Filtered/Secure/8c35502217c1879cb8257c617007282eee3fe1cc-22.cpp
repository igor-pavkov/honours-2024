bool Magick::Image::setColorMetric(const Image &reference_)
{
  bool
    status;

  Image
    ref=reference_;

  GetPPException;
  modifyImage();
  status=static_cast<bool>(SetImageColorMetric(image(),ref.constImage(),
    exceptionInfo));
  ThrowImageException;
  return(status);
}