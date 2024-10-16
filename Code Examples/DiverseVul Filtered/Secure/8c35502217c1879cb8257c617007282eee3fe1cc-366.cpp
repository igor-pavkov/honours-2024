void Magick::Image::page(const Magick::Geometry &pageSize_)
{
  modifyImage();
  options()->page(pageSize_);
  image()->page=pageSize_;
}