void Magick::Image::reduceNoise(const size_t order_)
{
  MagickCore::Image
    *newImage;

  GetPPException;
  newImage=StatisticImage(constImage(),NonpeakStatistic,order_,
    order_,exceptionInfo);
  replaceImage(newImage);
  ThrowImageException;
}