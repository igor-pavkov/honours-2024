void Magick::Image::subImage(const size_t subImage_)
{
  modifyImage();
  options()->subImage(subImage_);
}