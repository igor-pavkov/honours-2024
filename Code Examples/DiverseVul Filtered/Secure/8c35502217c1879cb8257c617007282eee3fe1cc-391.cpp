void Magick::Image::liquidRescale(const Geometry &geometry_)
{
  MagickCore::Image
    *newImage;

  size_t
    height=rows(),
    width=columns();

  ssize_t
    x=0,
    y=0;

  ParseMetaGeometry(static_cast<std::string>(geometry_).c_str(),&x,&y,&width,
    &height);

  GetPPException;
  newImage=LiquidRescaleImage(image(),width,height,x,y,exceptionInfo);
  replaceImage(newImage);
  ThrowImageException;
}