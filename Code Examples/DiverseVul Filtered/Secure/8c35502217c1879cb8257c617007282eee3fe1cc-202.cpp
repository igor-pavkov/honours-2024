void Magick::Image::grayscale(const PixelIntensityMethod method_)
{
  modifyImage();
  GetPPException;
  (void) GrayscaleImage(image(),method_,exceptionInfo);
  ThrowImageException;
}