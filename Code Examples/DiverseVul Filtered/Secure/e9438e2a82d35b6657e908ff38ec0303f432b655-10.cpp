MagickExport const StringInfo *GetImageProfile(const Image *image,
  const char *name)
{
  char
    key[MaxTextExtent];

  const StringInfo
    *profile;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  if (image->profiles == (SplayTreeInfo *) NULL)
    return((StringInfo *) NULL);
  (void) CopyMagickString(key,name,MaxTextExtent);
  profile=(const StringInfo *) GetValueFromSplayTree((SplayTreeInfo *)
    image->profiles,key);
  return(profile);
}