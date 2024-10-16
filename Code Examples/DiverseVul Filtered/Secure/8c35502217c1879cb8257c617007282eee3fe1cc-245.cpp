Magick::Geometry Magick::Image::montageGeometry(void) const
{
  if (constImage()->montage)
    return Magick::Geometry(constImage()->montage);

  if (!quiet())
    throwExceptionExplicit(MagickCore::CorruptImageWarning,
    "Image does not contain a montage");

  return(Magick::Geometry());
}