Magick::Image::Image(const Image &image_,const Geometry &geometry_)
  : _imgRef(new ImageRef)
{
  const RectangleInfo
    geometry=geometry_;

  OffsetInfo
    offset;

  MagickCore::Image
    *image;

  GetPPException;
  image=CloneImage(image_.constImage(),geometry_.width(),geometry_.height(),
    MagickTrue,exceptionInfo);
  replaceImage(image);
  _imgRef->options(new Options(*image_.constOptions()));
  offset.x=0;
  offset.y=0;
  (void) CopyImagePixels(image,image_.constImage(),&geometry,&offset,
    exceptionInfo);
  ThrowImageException;
}