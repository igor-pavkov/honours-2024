void Magick::Image::frame(const size_t width_,const size_t height_,
  const ssize_t innerBevel_,const ssize_t outerBevel_)
{
  FrameInfo
    info;

  MagickCore::Image
    *newImage;

  info.x=static_cast<ssize_t>(width_);
  info.y=static_cast<ssize_t>(height_);
  info.width=columns() + (static_cast<size_t>(info.x) << 1);
  info.height=rows() + (static_cast<size_t>(info.y) << 1);
  info.outer_bevel=static_cast<ssize_t>(outerBevel_);
  info.inner_bevel=static_cast<ssize_t>(innerBevel_);

  GetPPException;
  newImage=FrameImage(constImage(),&info,image()->compose,exceptionInfo);
  replaceImage(newImage);
  ThrowImageException;
}