void Magick::Image::annotate(const std::string &text_,
  const Geometry &boundingArea_,const GravityType gravity_)
{
  annotate(text_,boundingArea_,gravity_,0.0);
}