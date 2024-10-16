MagickExport StringInfo *RemoveImageProfile(Image *image,const char *name)
{
  StringInfo
    *profile;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  if (image->profiles == (SplayTreeInfo *) NULL)
    return((StringInfo *) NULL);
  profile=(StringInfo *) RemoveNodeFromSplayTree((SplayTreeInfo *)
    image->profiles,name);
  return(profile);
}