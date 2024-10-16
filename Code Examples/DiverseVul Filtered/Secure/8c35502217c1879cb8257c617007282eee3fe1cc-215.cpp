void Magick::Image::strokeLineCap(const Magick::LineCap lineCap_)
{
  modifyImage();
  options()->strokeLineCap(lineCap_);
}