void Magick::Image::brightnessContrast(const double brightness_,
  const double contrast_)
{
  modifyImage();
  GetPPException;
  BrightnessContrastImage(image(),brightness_,contrast_,exceptionInfo);
  ThrowImageException;
}