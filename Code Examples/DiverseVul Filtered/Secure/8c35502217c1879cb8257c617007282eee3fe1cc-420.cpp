void Magick::Image::backgroundTexture(const std::string &backgroundTexture_)
{
  modifyImage();
  options()->backgroundTexture(backgroundTexture_);
}