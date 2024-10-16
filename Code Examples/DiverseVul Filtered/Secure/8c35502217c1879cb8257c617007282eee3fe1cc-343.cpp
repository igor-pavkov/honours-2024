void Magick::Image::inverseFourierTransform(const Image &phase_,
  const bool magnitude_)
{
  MagickCore::Image
    *newImage;

  GetPPException;
  newImage=InverseFourierTransformImage(constImage(),phase_.constImage(),
    magnitude_ == true ? MagickTrue : MagickFalse,exceptionInfo);
  replaceImage(newImage);
  ThrowImageException;
}