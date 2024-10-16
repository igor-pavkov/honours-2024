void Magick::Image::transformSkewX(const double skewx_)
{
  modifyImage();
  options()->transformSkewX(skewx_);
}