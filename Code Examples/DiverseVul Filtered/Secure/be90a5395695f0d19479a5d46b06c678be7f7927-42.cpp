MagickExport MagickBooleanType ConcatenateString(char **destination,
  const char *source)
{
  size_t
    destination_length,
    length,
    source_length;

  assert(destination != (char **) NULL);
  if (source == (const char *) NULL)
    return(MagickTrue);
  if (*destination == (char *) NULL)
    {
      *destination=AcquireString(source);
      return(MagickTrue);
    }
  destination_length=strlen(*destination);
  source_length=strlen(source);
  length=destination_length;
  if (~length < source_length)
    ThrowFatalException(ResourceLimitFatalError,"UnableToConcatenateString");
  length+=source_length;
  if (~length < MagickPathExtent)
    ThrowFatalException(ResourceLimitFatalError,"UnableToConcatenateString");
  *destination=(char *) ResizeQuantumMemory(*destination,length+
    MagickPathExtent,sizeof(**destination));
  if (*destination == (char *) NULL)
    ThrowFatalException(ResourceLimitFatalError,"UnableToConcatenateString");
  if (source_length != 0)
    (void) memcpy((*destination)+destination_length,source,source_length);
  (*destination)[length]='\0';
  return(MagickTrue);
}