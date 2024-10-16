void Magick::Image::fontFamily(const std::string &family_)
{
  modifyImage();
  options()->fontFamily(family_);
}