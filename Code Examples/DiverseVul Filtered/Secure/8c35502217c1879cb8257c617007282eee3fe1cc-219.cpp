std::string Magick::Image::baseFilename(void) const
{
  return(std::string(constImage()->magick_filename));
}