void Magick::Image::splice(const Geometry &geometry_,
  const Color &backgroundColor_)
{
  backgroundColor(backgroundColor_);
  splice(geometry_);
}