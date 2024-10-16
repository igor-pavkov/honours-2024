void Magick::Image::floodFillColor(const Geometry &point_,
  const Magick::Color &fillColor_,const bool invert_)
{
  floodFillColor(point_.xOff(),point_.yOff(),fillColor_,invert_);
}