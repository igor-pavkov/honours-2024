MagickExport char *CloneString(char **destination,const char *source)
{
  size_t
    length;

  assert(destination != (char **) NULL);
  if (source == (const char *) NULL)
    {
      if (*destination != (char *) NULL)
        *destination=DestroyString(*destination);
      return(*destination);
    }
  if (*destination == (char *) NULL)
    {
      *destination=AcquireString(source);
      return(*destination);
    }
  length=strlen(source);
  if (~length < MagickPathExtent)
    ThrowFatalException(ResourceLimitFatalError,"UnableToAcquireString");
  *destination=(char *) ResizeQuantumMemory(*destination,length+
    MagickPathExtent,sizeof(**destination));
  if (*destination == (char *) NULL)
    ThrowFatalException(ResourceLimitFatalError,"UnableToAcquireString");
  if (length != 0)
    (void) memcpy(*destination,source,length*sizeof(**destination));
  (*destination)[length]='\0';
  return(*destination);
}