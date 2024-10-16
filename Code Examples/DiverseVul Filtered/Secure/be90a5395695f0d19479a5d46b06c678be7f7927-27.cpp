MagickExport int CompareStringInfo(const StringInfo *target,
  const StringInfo *source)
{
  int
    status;

  assert(target != (StringInfo *) NULL);
  assert(target->signature == MagickCoreSignature);
  assert(source != (StringInfo *) NULL);
  assert(source->signature == MagickCoreSignature);
  status=memcmp(target->datum,source->datum,MagickMin(target->length,
    source->length));
  if (status != 0)
    return(status);
  if (target->length == source->length)
    return(0);
  return(target->length < source->length ? -1 : 1);
}