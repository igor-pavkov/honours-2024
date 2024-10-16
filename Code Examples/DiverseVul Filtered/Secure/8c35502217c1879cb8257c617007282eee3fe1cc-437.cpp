void Magick::Image::isValid(const bool isValid_)
{
  if (!isValid_)
    {
      delete _imgRef;
      _imgRef=new ImageRef;
    }
  else if (!isValid())
    {
      // Construct with single-pixel black image to make
      // image valid. This is an obvious hack.
      size(Geometry(1,1));
      read("xc:black");
    }
}