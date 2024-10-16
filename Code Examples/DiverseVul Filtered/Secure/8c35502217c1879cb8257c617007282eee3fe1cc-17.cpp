void Magick::Image::extent(const Geometry &geometry_,
  const Color &backgroundColor_)
{
  backgroundColor(backgroundColor_);
  extent(geometry_);
}