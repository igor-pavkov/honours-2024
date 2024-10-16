Magick::Image::Image(const std::string &imageSpec_)
  : _imgRef(new ImageRef)
{
  try
  {
    // Initialize, Allocate and Read images
    quiet(true);
    read(imageSpec_);
    quiet(false);
  }
  catch(const Error&)
  {
    // Release resources
    delete _imgRef;
    throw;
  }
}