Magick::Color Magick::Image::colorMap(const size_t index_) const
{
  if (!constImage()->colormap)
    {
      throwExceptionExplicit(MagickCore::OptionError,
        "Image does not contain a colormap");
      return(Color());
    }

  if (index_ > constImage()->colors-1)
    throwExceptionExplicit(MagickCore::OptionError,"Index out of range");

  return(Magick::Color((constImage()->colormap)[index_]));
}