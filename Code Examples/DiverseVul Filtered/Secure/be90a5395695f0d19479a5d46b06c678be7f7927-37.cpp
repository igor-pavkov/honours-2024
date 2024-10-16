MagickExport char *ConstantString(const char *source)
{
  char
    *destination;

  size_t
    length;

  length=0;
  if (source != (char *) NULL)
    length+=strlen(source);
  destination=(char *) NULL;
  if (~length >= 1UL)
    destination=(char *) AcquireQuantumMemory(length+1UL,sizeof(*destination));
  if (destination == (char *) NULL)
    ThrowFatalException(ResourceLimitFatalError,"UnableToAcquireString");
  *destination='\0';
  if (source != (char *) NULL)
    (void) memcpy(destination,source,length*sizeof(*destination));
  destination[length]='\0';
  return(destination);
}