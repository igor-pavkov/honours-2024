void Magick::Image::boxColor(const Color &boxColor_)
{
  modifyImage();
  options()->boxColor(boxColor_);
}