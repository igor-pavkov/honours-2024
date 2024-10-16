void Magick::Image::strokeWidth(const double strokeWidth_)
{
  char
    value[MagickPathExtent];

  modifyImage();
  options()->strokeWidth(strokeWidth_);
  FormatLocaleString(value,MagickPathExtent,"%.20g",strokeWidth_);
  (void) SetImageArtifact(image(),"strokewidth",value);
}