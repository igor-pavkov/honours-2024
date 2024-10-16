const Magick::Quantum *Magick::Image::getConstPixels(const ssize_t x_,
  const ssize_t y_,const size_t columns_,const size_t rows_) const
{
  const Quantum
    *p;

  GetPPException;
  p=GetVirtualPixels(constImage(),x_, y_,columns_, rows_,exceptionInfo);
  ThrowImageException;
  return(p);
}