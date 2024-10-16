double Magick::Image::compare(const Image &reference_,const MetricType metric_)
{
  double
    distortion=0.0;

  GetPPException;
  GetImageDistortion(image(),reference_.constImage(),metric_,&distortion,
    exceptionInfo);
  ThrowImageException;
  return(distortion);
}