void Magick::Image::endian(const Magick::EndianType endian_)
{
  modifyImage();
  options()->endian(endian_);
  image()->endian=endian_;
}