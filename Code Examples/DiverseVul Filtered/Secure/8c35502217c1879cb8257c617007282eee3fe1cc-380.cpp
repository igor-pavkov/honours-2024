void Magick::Image::copyPixels(const Image &source_,const Geometry &geometry_,
  const Offset &offset_)
{
  const OffsetInfo
    offset=offset_;

  const RectangleInfo
    geometry=geometry_;

  GetPPException;
  (void) CopyImagePixels(image(),source_.constImage(),&geometry,&offset,
    exceptionInfo);
  ThrowImageException;
}