void Magick::Image::fillRule(const Magick::FillRule &fillRule_)
{
  modifyImage();
  options()->fillRule(fillRule_);
}