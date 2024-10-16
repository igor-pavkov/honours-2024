MagickExport char *FileToString(const char *filename,const size_t extent,
  ExceptionInfo *exception)
{
  size_t
    length;

  assert(filename != (const char *) NULL);
  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",filename);
  assert(exception != (ExceptionInfo *) NULL);
  return((char *) FileToBlob(filename,extent,&length,exception));
}