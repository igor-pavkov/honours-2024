void Magick::Image::fontWeight(const size_t weight_)
{
  modifyImage();
  options()->fontWeight(weight_);
}