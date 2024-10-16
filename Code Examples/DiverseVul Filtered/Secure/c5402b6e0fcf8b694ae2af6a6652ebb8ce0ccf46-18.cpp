MagickExport Image *ResampleImage(const Image *image,const double x_resolution,
  const double y_resolution,const FilterType filter,ExceptionInfo *exception)
{
#define ResampleImageTag  "Resample/Image"

  Image
    *resample_image;

  size_t
    height,
    width;

  /*
    Initialize sampled image attributes.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  width=(size_t) (x_resolution*image->columns/(image->resolution.x == 0.0 ?
    72.0 : image->resolution.x)+0.5);
  height=(size_t) (y_resolution*image->rows/(image->resolution.y == 0.0 ?
    72.0 : image->resolution.y)+0.5);
  resample_image=ResizeImage(image,width,height,filter,exception);
  if (resample_image != (Image *) NULL)
    {
      resample_image->resolution.x=x_resolution;
      resample_image->resolution.y=y_resolution;
    }
  return(resample_image);
}