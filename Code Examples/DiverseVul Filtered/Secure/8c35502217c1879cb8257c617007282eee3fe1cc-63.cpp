void Magick::Image::extent(const Geometry &geometry_,
  const Color &backgroundColor_,const GravityType gravity_)
{
  backgroundColor(backgroundColor_);
  extent(geometry_,gravity_);
}