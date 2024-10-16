MagickExport void SetGeometry(const Image *image,RectangleInfo *geometry)
{
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(geometry != (RectangleInfo *) NULL);
  (void) memset(geometry,0,sizeof(*geometry));
  geometry->width=image->columns;
  geometry->height=image->rows;
}