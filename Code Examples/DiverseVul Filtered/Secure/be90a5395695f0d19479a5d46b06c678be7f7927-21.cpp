static StringInfo *AcquireStringInfoContainer()
{
  StringInfo
    *string_info;

  string_info=(StringInfo *) AcquireCriticalMemory(sizeof(*string_info));
  (void) memset(string_info,0,sizeof(*string_info));
  string_info->signature=MagickCoreSignature;
  return(string_info);
}