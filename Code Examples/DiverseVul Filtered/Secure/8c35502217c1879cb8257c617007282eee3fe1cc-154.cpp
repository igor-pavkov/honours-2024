Magick::ResolutionType Magick::Image::resolutionUnits(void) const
{
  return(static_cast<Magick::ResolutionType>(constImage()->units));
}