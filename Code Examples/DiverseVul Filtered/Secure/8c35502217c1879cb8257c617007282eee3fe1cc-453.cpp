void Magick::Image::composite(const Image &compositeImage_,
  const Geometry &offset_,const CompositeOperator compose_)
{
  size_t
    height=rows(),
    width=columns();

  ssize_t
    x=offset_.xOff(),
    y=offset_.yOff();

  ParseMetaGeometry(static_cast<std::string>(offset_).c_str(),&x,&y,&width,
    &height);

  modifyImage();
  GetPPException;
  CompositeImage(image(),compositeImage_.constImage(),compose_,MagickTrue,
    x,y,exceptionInfo);
  ThrowImageException;
}