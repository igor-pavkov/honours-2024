Magick::Image::Image(const Image &image_)
  : _imgRef(image_._imgRef)
{
  _imgRef->increase();
}