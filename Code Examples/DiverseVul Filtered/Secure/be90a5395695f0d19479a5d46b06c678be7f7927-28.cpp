MagickExport StringInfo *AcquireStringInfo(const size_t length)
{
  StringInfo
    *string_info;

  string_info=AcquireStringInfoContainer();
  string_info->length=length;
  if (~string_info->length >= (MagickPathExtent-1))
    string_info->datum=(unsigned char *) AcquireQuantumMemory(
      string_info->length+MagickPathExtent,sizeof(*string_info->datum));
  if (string_info->datum == (unsigned char *) NULL)
    ThrowFatalException(ResourceLimitFatalError,"MemoryAllocationFailed");
  (void) memset(string_info->datum,0,(length+MagickPathExtent)*
    sizeof(*string_info->datum));
  return(string_info);
}