void Magick::Image::textEncoding(const std::string &encoding_)
{
  modifyImage();
  options()->textEncoding(encoding_);
}