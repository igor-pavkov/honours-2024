void Magick::Image::fontStyle(const StyleType pointSize_)
{
  modifyImage();
  options()->fontStyle(pointSize_);
}