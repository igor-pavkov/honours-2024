void Magick::Image::attenuate(const double attenuate_)
{
  char
    value[MagickPathExtent];

  modifyImage();
  FormatLocaleString(value,MagickPathExtent,"%.20g",attenuate_);
  (void) SetImageArtifact(image(),"attenuate",value);
}