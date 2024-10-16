void Magick::Image::medianFilter(const double radius_)
{
  MagickCore::Image
    *newImage;

  GetPPException;
  newImage=StatisticImage(image(),MedianStatistic,(size_t) radius_,
    (size_t) radius_,exceptionInfo);
  replaceImage(newImage);
  ThrowImageException;
}