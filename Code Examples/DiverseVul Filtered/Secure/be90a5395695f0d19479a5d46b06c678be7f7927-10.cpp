MagickExport size_t GetStringInfoLength(const StringInfo *string_info)
{
  assert(string_info != (StringInfo *) NULL);
  assert(string_info->signature == MagickCoreSignature);
  return(string_info->length);
}