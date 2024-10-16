Magick::Quantum *Magick::Image::getPixels(const ssize_t x_,const ssize_t y_,
  const size_t columns_,const size_t rows_)
{
  Quantum
    *result;

  modifyImage();
  GetPPException;
  result=GetAuthenticPixels(image(),x_, y_,columns_,rows_,exceptionInfo);
  ThrowImageException;

  return(result);
}