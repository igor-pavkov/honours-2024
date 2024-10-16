MagickExport Image *DeconstructImages(const Image *images,
  ExceptionInfo *exception)
{
  return(CompareImageLayers(images,CompareAnyLayer,exception));
}