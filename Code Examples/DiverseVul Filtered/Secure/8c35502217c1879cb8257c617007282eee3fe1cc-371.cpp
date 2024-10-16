void Magick::Image::meanShift(const size_t width_,const size_t height_,
  const double color_distance_)
{
  MagickCore::Image
    *newImage;

  GetPPException;
  newImage=MeanShiftImage(constImage(),width_,height_,color_distance_,
    exceptionInfo);
  replaceImage(newImage);
  ThrowImageException;
}