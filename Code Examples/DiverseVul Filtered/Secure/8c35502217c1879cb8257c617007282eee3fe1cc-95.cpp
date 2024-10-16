Magick::Geometry Magick::Image::geometry(void) const
{
  if (constImage()->geometry)
    return Geometry(constImage()->geometry);

  if (!quiet())
    throwExceptionExplicit(MagickCore::OptionWarning,
      "Image does not contain a geometry");

  return(Geometry());
}