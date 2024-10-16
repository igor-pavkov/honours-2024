Magick::Image Magick::Image::compare(const Image &reference_,
  const MetricType metric_,double *distortion)
{
  MagickCore::Image
    *newImage;

  GetPPException;
  newImage=CompareImages(image(),reference_.constImage(),metric_,distortion,
    exceptionInfo);
  ThrowImageException;
  if (newImage == (MagickCore::Image *) NULL)
    return(Magick::Image());
  else
    return(Magick::Image(newImage));
}