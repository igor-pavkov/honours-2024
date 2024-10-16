void Magick::Image::floodFillColor(const Geometry &point_,
  const Magick::Color &fillColor_,const Magick::Color &borderColor_,
  const bool invert_)
{
  floodFillColor(point_.xOff(),point_.yOff(),fillColor_,borderColor_,invert_);
}