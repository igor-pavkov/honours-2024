void Magick::Image::wave(const double amplitude_,const double wavelength_)
{
  MagickCore::Image
    *newImage;

  GetPPException;
  newImage=WaveImage(constImage(),amplitude_,wavelength_,image()->interpolate,
    exceptionInfo);
  replaceImage(newImage);
  ThrowImageException;
}