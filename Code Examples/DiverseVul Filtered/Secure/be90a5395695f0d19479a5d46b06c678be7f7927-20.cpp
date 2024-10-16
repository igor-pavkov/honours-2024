MagickExport StringInfo *SplitStringInfo(StringInfo *string_info,
  const size_t offset)
{
  StringInfo
    *split_info;

  assert(string_info != (StringInfo *) NULL);
  assert(string_info->signature == MagickCoreSignature);
  if (offset > string_info->length)
    return((StringInfo *) NULL);
  split_info=AcquireStringInfo(offset);
  SetStringInfo(split_info,string_info);
  (void) memmove(string_info->datum,string_info->datum+offset,
    string_info->length-offset+MagickPathExtent);
  SetStringInfoLength(string_info,string_info->length-offset);
  return(split_info);
}