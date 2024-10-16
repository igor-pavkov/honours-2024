bool Magick::Image::compare(const Image &reference_) const
{
  bool
    status;

  Image
    ref=reference_;

  GetPPException;
  status=static_cast<bool>(IsImagesEqual(constImage(),ref.constImage(),
    exceptionInfo));
  ThrowImageException;
  return(status);
}