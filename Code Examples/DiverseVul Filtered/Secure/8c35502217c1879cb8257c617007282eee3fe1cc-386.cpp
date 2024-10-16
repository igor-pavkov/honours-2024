void Magick::Image::sigmoidalContrast(const bool sharpen_,
  const double contrast,const double midpoint)
{
  modifyImage();
  GetPPException;
  (void) SigmoidalContrastImage(image(),(MagickBooleanType) sharpen_,contrast,
    midpoint,exceptionInfo);
  ThrowImageException;
}