void Magick::Image::size(const Geometry &geometry_)
{
  modifyImage();
  options()->size(geometry_);
  image()->rows=geometry_.height();
  image()->columns=geometry_.width();
}