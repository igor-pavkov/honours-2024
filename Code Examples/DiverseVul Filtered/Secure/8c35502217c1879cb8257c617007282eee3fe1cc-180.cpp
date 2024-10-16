Magick::Image::Image(const Geometry &size_,const Color &color_)
  : _imgRef(new ImageRef)
{
  // xc: prefix specifies an X11 color string
  std::string imageSpec("xc:");
  imageSpec+=color_;

  try
  {
    quiet(true);
    // Set image size
    size(size_);

    // Initialize, Allocate and Read images
    read(imageSpec);
    quiet(false);
  }
  catch(const Error&)
  {
    // Release resources
    delete _imgRef;
    throw;
  }
}