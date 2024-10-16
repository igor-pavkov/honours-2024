MagickExport void SetStringInfoLength(StringInfo *string_info,
  const size_t length)
{
  assert(string_info != (StringInfo *) NULL);
  assert(string_info->signature == MagickCoreSignature);
  if (string_info->length == length)
    return;
  if (~length < MagickPathExtent)
    ThrowFatalException(ResourceLimitFatalError,"MemoryAllocationFailed");
  string_info->length=length;
  if (string_info->datum == (unsigned char *) NULL)
    string_info->datum=(unsigned char *) AcquireQuantumMemory(length+
      MagickPathExtent,sizeof(*string_info->datum));
  else
    string_info->datum=(unsigned char *) ResizeQuantumMemory(string_info->datum,
      length+MagickPathExtent,sizeof(*string_info->datum));
  if (string_info->datum == (unsigned char *) NULL)
    ThrowFatalException(ResourceLimitFatalError,"MemoryAllocationFailed");
}