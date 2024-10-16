Magick::Geometry Magick::Image::boundingBox(void) const
{
  RectangleInfo
    bbox;

  GetPPException;
  bbox=GetImageBoundingBox(constImage(),exceptionInfo);
  ThrowImageException;
  return(Geometry(bbox));
}