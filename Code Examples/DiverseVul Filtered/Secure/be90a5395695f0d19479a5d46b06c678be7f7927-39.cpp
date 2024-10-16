MagickExport void SetStringInfoName(StringInfo *string_info,const char *name)
{
  assert(string_info != (StringInfo *) NULL);
  assert(string_info->signature == MagickCoreSignature);
  assert(name != (const char *) NULL);
  string_info->name=ConstantString(name);
}