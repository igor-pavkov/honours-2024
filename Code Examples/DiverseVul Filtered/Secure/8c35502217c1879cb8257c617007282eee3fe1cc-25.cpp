std::string Magick::Image::directory(void) const
{
  if (constImage()->directory)
    return(std::string(constImage()->directory));

  if (!quiet())
    throwExceptionExplicit(MagickCore::CorruptImageWarning,
      "Image does not contain a directory");

  return(std::string());
}