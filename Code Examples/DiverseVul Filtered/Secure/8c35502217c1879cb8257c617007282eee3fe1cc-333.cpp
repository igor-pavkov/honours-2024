void Magick::Image::resample(const Point &density_)
{
  MagickCore::Image
    *newImage;

  GetPPException;
  newImage=ResampleImage(constImage(),density_.x(),density_.y(),
    image()->filter,exceptionInfo);
  replaceImage(newImage);
  ThrowImageException;
}