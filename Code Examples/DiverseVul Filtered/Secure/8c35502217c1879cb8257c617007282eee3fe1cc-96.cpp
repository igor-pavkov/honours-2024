void Magick::Image::read(const Blob &blob_,const Geometry &size_)
{
  size(size_);
  read(blob_);
}