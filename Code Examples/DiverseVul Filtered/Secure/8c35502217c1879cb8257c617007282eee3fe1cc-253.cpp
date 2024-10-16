void Magick::Image::extent(const Geometry &geometry_,
  const GravityType gravity_)
{
  RectangleInfo
    geometry;

  SetGeometry(image(),&geometry);
  geometry.width=geometry_.width();
  geometry.height=geometry_.height();
  GravityAdjustGeometry(image()->columns,image()->rows,gravity_,&geometry);
  extent(geometry);
}