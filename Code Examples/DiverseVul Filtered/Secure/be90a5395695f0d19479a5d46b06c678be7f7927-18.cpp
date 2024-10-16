MagickExport void ResetStringInfo(StringInfo *string_info)
{
  assert(string_info != (StringInfo *) NULL);
  assert(string_info->signature == MagickCoreSignature);
  (void) memset(string_info->datum,0,string_info->length);
}