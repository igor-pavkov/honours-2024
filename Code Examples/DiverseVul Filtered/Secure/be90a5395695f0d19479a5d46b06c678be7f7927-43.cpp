MagickExport StringInfo *DestroyStringInfo(StringInfo *string_info)
{
  assert(string_info != (StringInfo *) NULL);
  assert(string_info->signature == MagickCoreSignature);
  if (string_info->datum != (unsigned char *) NULL)
    string_info->datum=(unsigned char *) RelinquishMagickMemory(
      string_info->datum);
  if (string_info->name != (char *) NULL)
    string_info->name=DestroyString(string_info->name);
  if (string_info->path != (char *) NULL)
    string_info->path=DestroyString(string_info->path);
  string_info->signature=(~MagickCoreSignature);
  string_info=(StringInfo *) RelinquishMagickMemory(string_info);
  return(string_info);
}