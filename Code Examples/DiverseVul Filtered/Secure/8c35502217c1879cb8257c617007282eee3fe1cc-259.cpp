void Magick::Image::annotate(const std::string &text_,
  const Geometry &location_)
{
  annotate(text_,location_,NorthWestGravity,0.0);
}