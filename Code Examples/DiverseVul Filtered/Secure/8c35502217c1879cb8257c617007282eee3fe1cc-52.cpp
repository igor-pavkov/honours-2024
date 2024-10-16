void Magick::Image::resize(const Geometry &geometry_)
{
  MagickCore::Image
    *newImage;

  size_t
    height=rows(),
    width=columns();

  ssize_t
    x=0,
    y=0;

  // Calculate new size.  This code should be supported using binary arguments
  // in the ImageMagick library.
  ParseMetaGeometry(static_cast<std::string>(geometry_).c_str(),&x,&y,&width,
    &height);

  GetPPException;
  newImage=ResizeImage(constImage(),width,height,image()->filter,
    exceptionInfo);
  replaceImage(newImage);
  ThrowImageException;
}