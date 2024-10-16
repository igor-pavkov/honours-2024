MagickExport void SetStringInfo(StringInfo *string_info,
  const StringInfo *source)
{
  assert(string_info != (StringInfo *) NULL);
  assert(string_info->signature == MagickCoreSignature);
  assert(source != (StringInfo *) NULL);
  assert(source->signature == MagickCoreSignature);
  if (string_info->length == 0)
    return;
  (void) memset(string_info->datum,0,string_info->length);
  (void) memcpy(string_info->datum,source->datum,MagickMin(string_info->length,
    source->length));
}