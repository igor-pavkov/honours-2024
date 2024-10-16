size_t Magick::Image::colorMapSize(void) const
{
  if (!constImage()->colormap)
    throwExceptionExplicit(MagickCore::OptionError,
      "Image does not contain a colormap");

  return(constImage()->colors);
}