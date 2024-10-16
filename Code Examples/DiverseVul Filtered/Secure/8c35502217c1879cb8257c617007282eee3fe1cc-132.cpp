void Magick::Image::textKerning(double kerning_)
{
  modifyImage();
  options()->textKerning(kerning_);
}