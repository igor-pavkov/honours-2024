Magick::Image::Image(const Blob &blob_,const Geometry &size_,
  const std::string &magick_)
  : _imgRef(new ImageRef)
{
  try
  {
    // Read from Blob
    quiet(true);
    read(blob_,size_,magick_);
    quiet(false);
  }
  catch(const Error&)
  {
    // Release resources
    delete _imgRef;
    throw;
  }
}