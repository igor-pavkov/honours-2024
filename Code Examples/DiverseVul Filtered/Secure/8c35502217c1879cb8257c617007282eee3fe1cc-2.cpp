void Magick::Image::strokeLineJoin(const Magick::LineJoin lineJoin_)
{
  modifyImage();
  options()->strokeLineJoin(lineJoin_);
}