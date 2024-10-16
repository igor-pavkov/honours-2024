MagickExport Image *ExtentImage(const Image *image,
  const RectangleInfo *geometry,ExceptionInfo *exception)
{
  Image
    *extent_image;

  /*
    Allocate extent image.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(geometry != (const RectangleInfo *) NULL);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  if ((image->columns == geometry->width) &&
      (image->rows == geometry->height) &&
      (geometry->x == 0) && (geometry->y == 0))
    return(CloneImage(image,0,0,MagickTrue,exception));
  extent_image=CloneImage(image,geometry->width,geometry->height,MagickTrue,
    exception);
  if (extent_image == (Image *) NULL)
    return((Image *) NULL);
  (void) SetImageBackgroundColor(extent_image,exception);
  (void) CompositeImage(extent_image,image,image->compose,MagickTrue,
    -geometry->x,-geometry->y,exception);
  return(extent_image);
}