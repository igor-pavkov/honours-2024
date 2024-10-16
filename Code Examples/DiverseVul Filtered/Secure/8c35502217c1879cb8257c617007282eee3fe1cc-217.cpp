void Magick::Image::frame(const Geometry &geometry_)
{
  FrameInfo
    info;
  
  MagickCore::Image
    *newImage;

  info.x=static_cast<ssize_t>(geometry_.width());
  info.y=static_cast<ssize_t>(geometry_.height());
  info.width=columns() + (static_cast<size_t>(info.x) << 1);
  info.height=rows() + (static_cast<size_t>(info.y) << 1);
  info.outer_bevel=geometry_.xOff();
  info.inner_bevel=geometry_.yOff();

  GetPPException;
  newImage=FrameImage(constImage(),&info,image()->compose,exceptionInfo);
  replaceImage(newImage);
  ThrowImageException;
}