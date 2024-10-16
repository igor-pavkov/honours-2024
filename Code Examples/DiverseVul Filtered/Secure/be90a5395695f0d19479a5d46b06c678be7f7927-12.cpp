MagickExport StringInfo *FileToStringInfo(const char *filename,
  const size_t extent,ExceptionInfo *exception)
{
  StringInfo
    *string_info;

  assert(filename != (const char *) NULL);
  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",filename);
  assert(exception != (ExceptionInfo *) NULL);
  string_info=AcquireStringInfoContainer();
  string_info->path=ConstantString(filename);
  string_info->datum=(unsigned char *) FileToBlob(filename,extent,
    &string_info->length,exception);
  if (string_info->datum == (unsigned char *) NULL)
    {
      string_info=DestroyStringInfo(string_info);
      return((StringInfo *) NULL);
    }
  return(string_info);
}