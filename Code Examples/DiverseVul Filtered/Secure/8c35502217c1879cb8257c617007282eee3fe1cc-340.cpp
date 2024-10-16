void Magick::Image::transformSkewY(const double skewy_)
{
  modifyImage();
  options()->transformSkewY(skewy_);
}