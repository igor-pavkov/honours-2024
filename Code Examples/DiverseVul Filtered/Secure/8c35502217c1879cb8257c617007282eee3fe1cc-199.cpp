void Magick::Image::compressType(const CompressionType compressType_)
{
  modifyImage();
  image()->compression=compressType_;
  options()->compressType(compressType_);
}