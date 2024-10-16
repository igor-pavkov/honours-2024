MagickExport char *AcquireString(const char *source)
{
  char
    *destination;

  size_t
    length;

  length=0;
  if (source != (char *) NULL)
    length+=strlen(source);
  if (~length < MagickPathExtent)
    ThrowFatalException(ResourceLimitFatalError,"UnableToAcquireString");
  destination=(char *) AcquireQuantumMemory(length+MagickPathExtent,
    sizeof(*destination));
  if (destination == (char *) NULL)
    ThrowFatalException(ResourceLimitFatalError,"UnableToAcquireString");
  *destination='\0';
  if (source != (char *) NULL)
    (void) memcpy(destination,source,length*sizeof(*destination));
  destination[length]='\0';
  return(destination);
}