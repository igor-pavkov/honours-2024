std::string Magick::Image::artifact(const std::string &name_) const
{
  const char
    *value;

  value=GetImageArtifact(constImage(),name_.c_str());
  if (value)
    return(std::string(value));
  return(std::string());
}