MagickExport void SetStringInfoPath(StringInfo *string_info,const char *path)
{
  assert(string_info != (StringInfo *) NULL);
  assert(string_info->signature == MagickCoreSignature);
  assert(path != (const char *) NULL);
  string_info->path=ConstantString(path);
}