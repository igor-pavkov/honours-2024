void Magick::Image::splice(const Geometry &geometry_,
  const Color &backgroundColor_,const GravityType gravity_)
{
  backgroundColor(backgroundColor_);
  image()->gravity=gravity_;
  splice(geometry_);
}