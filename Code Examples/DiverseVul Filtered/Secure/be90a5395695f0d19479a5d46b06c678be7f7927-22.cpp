MagickExport char *StringInfoToString(const StringInfo *string_info)
{
  char
    *string;

  size_t
    length;

  string=(char *) NULL;
  length=string_info->length;
  if (~length >= (MagickPathExtent-1))
    string=(char *) AcquireQuantumMemory(length+MagickPathExtent,
      sizeof(*string));
  if (string == (char *) NULL)
    return((char *) NULL);
  (void) memcpy(string,(char *) string_info->datum,length*sizeof(*string));
  string[length]='\0';
  return(string);
}