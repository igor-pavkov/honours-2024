MagickExport void SetStringInfoDatum(StringInfo *string_info,
  const unsigned char *source)
{
  assert(string_info != (StringInfo *) NULL);
  assert(string_info->signature == MagickCoreSignature);
  if (string_info->length != 0)
    (void) memcpy(string_info->datum,source,string_info->length);
}