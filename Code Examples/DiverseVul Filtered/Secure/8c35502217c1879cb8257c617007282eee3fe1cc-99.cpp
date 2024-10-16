void Magick::Image::waveletDenoise(const double threshold_,
  const double softness_)
{
  MagickCore::Image
    *newImage;

  GetPPException;
  newImage=WaveletDenoiseImage(constImage(),threshold_,softness_,
    exceptionInfo);
  replaceImage(newImage);
  ThrowImageException;
}