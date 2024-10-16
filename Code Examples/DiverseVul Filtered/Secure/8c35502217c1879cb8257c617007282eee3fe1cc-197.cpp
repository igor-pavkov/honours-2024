void Magick::Image::strokeMiterLimit(const size_t strokeMiterLimit_)
{
  modifyImage();
  options()->strokeMiterLimit(strokeMiterLimit_);
}