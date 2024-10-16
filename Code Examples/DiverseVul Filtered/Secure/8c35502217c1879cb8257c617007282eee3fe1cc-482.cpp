Magick::Image::Image(const Blob &blob_)
  : _imgRef(new ImageRef)
{
  try
  {
    // Initialize, Allocate and Read images
    quiet(true);
    read(blob_);
    quiet(false);
  }
  catch (const Error&)
  {
    // Release resources
    delete _imgRef;
    throw;
  }
}