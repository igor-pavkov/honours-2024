MagickExport StringInfo *CloneStringInfo(const StringInfo *string_info)
{
  StringInfo
    *clone_info;

  assert(string_info != (StringInfo *) NULL);
  assert(string_info->signature == MagickCoreSignature);
  clone_info=AcquireStringInfo(string_info->length);
  if (string_info->length != 0)
    (void) memcpy(clone_info->datum,string_info->datum,string_info->length+1);
  return(clone_info);
}