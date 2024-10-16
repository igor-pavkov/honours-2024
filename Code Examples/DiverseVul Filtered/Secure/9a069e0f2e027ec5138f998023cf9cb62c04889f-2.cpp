static MagickBooleanType SetImageProfileInternal(Image *image,const char *name,
  const StringInfo *profile,const MagickBooleanType recursive,
  ExceptionInfo *exception)
{
  char
    key[MagickPathExtent];

  MagickBooleanType
    status;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  if (image->profiles == (SplayTreeInfo *) NULL)
    image->profiles=NewSplayTree(CompareSplayTreeString,RelinquishMagickMemory,
      DestroyProfile);
  (void) CopyMagickString(key,name,MagickPathExtent);
  LocaleLower(key);
  status=AddValueToSplayTree((SplayTreeInfo *) image->profiles,
    ConstantString(key),CloneStringInfo(profile));
  if (status != MagickFalse)
    {
      if (LocaleCompare(name,"8bim") == 0)
        GetProfilesFromResourceBlock(image,profile,exception);
      else
        if (recursive == MagickFalse)
          WriteTo8BimProfile(image,name,profile);
    }
  return(status);
}