Magick::ClassType Magick::Image::classType(void) const
{
  return static_cast<Magick::ClassType>(constImage()->storage_class);
}