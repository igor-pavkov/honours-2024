std::string Magick::Image::magick(void) const
{
  if (*(constImage()->magick) != '\0')
    return(std::string(constImage()->magick));

  return(constOptions()->magick());
}