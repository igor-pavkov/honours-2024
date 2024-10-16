void Magick::Image::read(const Blob &blob_,const Geometry &size_,
  const size_t depth_)
{
  size(size_);
  depth(depth_);
  read(blob_);
}