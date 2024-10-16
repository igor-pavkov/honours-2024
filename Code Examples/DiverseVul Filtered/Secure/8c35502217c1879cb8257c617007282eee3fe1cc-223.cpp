Magick::Image::~Image()
{
  try
  {
    if (_imgRef->decrease() == 0)
      delete _imgRef;
  }
  catch(Magick::Exception)
  {
  }

  _imgRef=(Magick::ImageRef *) NULL;
}