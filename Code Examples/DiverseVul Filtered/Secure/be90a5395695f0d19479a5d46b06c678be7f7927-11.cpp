MagickExport StringInfo *StringToStringInfo(const char *string)
{
  StringInfo
    *string_info;

  assert(string != (const char *) NULL);
  string_info=AcquireStringInfo(strlen(string));
  SetStringInfoDatum(string_info,(const unsigned char *) string);
  return(string_info);
}