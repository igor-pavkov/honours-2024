Magick::Image Magick::Image::subImageSearch(const Image &reference_,
  const MetricType metric_,Geometry *offset_,double *similarityMetric_,
  const double similarityThreshold)
{
  MagickCore::Image
    *newImage;

  RectangleInfo
    offset;

  GetPPException;
  newImage=SimilarityImage(image(),reference_.constImage(),metric_,
    similarityThreshold,&offset,similarityMetric_,exceptionInfo);
  ThrowImageException;
  if (offset_ != (Geometry *) NULL)
    *offset_=offset;
  if (newImage == (MagickCore::Image *) NULL)
    return(Magick::Image());
  else
    return(Magick::Image(newImage));
}