Magick::Image::Image(const size_t width_,const size_t height_,
  const std::string &map_,const StorageType type_,const void *pixels_)
  : _imgRef(new ImageRef)
{
  try
  {
    quiet(true);
    read(width_,height_,map_.c_str(),type_,pixels_);
    quiet(false);
  }
  catch(const Error&)
  {
    // Release resources
    delete _imgRef;
    throw;
  }
}