void Magick::Image::write(const ssize_t x_,const ssize_t y_,
  const size_t columns_,const size_t rows_,const std::string &map_,
  const StorageType type_,void *pixels_)
{
  GetPPException;
  ExportImagePixels(image(),x_,y_,columns_,rows_,map_.c_str(),type_,pixels_,
    exceptionInfo);
  ThrowImageException;
}