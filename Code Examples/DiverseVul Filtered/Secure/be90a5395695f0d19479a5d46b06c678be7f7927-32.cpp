MagickExport unsigned char *GetStringInfoDatum(const StringInfo *string_info)
{
  assert(string_info != (StringInfo *) NULL);
  assert(string_info->signature == MagickCoreSignature);
  return(string_info->datum);
}