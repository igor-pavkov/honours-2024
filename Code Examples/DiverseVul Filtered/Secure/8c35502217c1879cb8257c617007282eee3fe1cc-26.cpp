Magick::Image::Image(MagickCore::Image *image_)
  : _imgRef(new ImageRef(image_))
{
}