void Magick::Image::shade(const double azimuth_,const double elevation_,
  const bool colorShading_)
{
  MagickCore::Image
    *newImage;

  GetPPException;
  newImage=ShadeImage(constImage(),colorShading_ == true ?
    MagickTrue : MagickFalse,azimuth_,elevation_,exceptionInfo);
  replaceImage(newImage);
  ThrowImageException;
}