void Magick::Image::quality(const size_t quality_)
{
  modifyImage();
  image()->quality=quality_;
  options()->quality(quality_);
}