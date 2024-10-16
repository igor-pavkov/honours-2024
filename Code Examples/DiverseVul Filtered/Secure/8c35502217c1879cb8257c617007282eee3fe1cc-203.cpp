void Magick::Image::colorFuzz(const double fuzz_)
{
  modifyImage();
  image()->fuzz=fuzz_;
  options()->colorFuzz(fuzz_);
}