void Magick::Image::read(const size_t width_,const size_t height_,
  const std::string &map_,const StorageType type_,const void *pixels_)
{
  MagickCore::Image
    *newImage;

  GetPPException;
  newImage=ConstituteImage(width_,height_,map_.c_str(),type_, pixels_,
    exceptionInfo);
  replaceImage(newImage);
  ThrowImageException;
}