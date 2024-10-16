void Magick::Image::filterType(const Magick::FilterType filterType_)
{
  modifyImage();
  image()->filter=filterType_;
}