void Magick::Image::interlaceType(const Magick::InterlaceType interlace_)
{
  modifyImage();
  image()->interlace=interlace_;
  options()->interlaceType(interlace_);
}