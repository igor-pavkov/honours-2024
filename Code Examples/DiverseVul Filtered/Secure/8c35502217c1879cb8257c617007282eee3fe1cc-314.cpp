Magick::Image::Image(const Blob &blob_,const Geometry &size_,
  const size_t depth_)
  : _imgRef(new ImageRef)
{
  try
  {
    // Read from Blob
    quiet(true);
    read(blob_,size_,depth_);
    quiet(false);
  }
  catch(const Error&)
  {
    // Release resources
    delete _imgRef;
    throw;
  }
}