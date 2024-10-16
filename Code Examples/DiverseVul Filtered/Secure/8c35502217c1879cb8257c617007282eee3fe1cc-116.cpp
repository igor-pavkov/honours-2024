void Magick::Image::roll(const size_t columns_,const size_t rows_)
{
  MagickCore::Image
    *newImage;

  GetPPException;
  newImage=RollImage(constImage(),static_cast<ssize_t>(columns_),
    static_cast<ssize_t>(rows_),exceptionInfo);
  replaceImage(newImage);
  ThrowImageException;
}