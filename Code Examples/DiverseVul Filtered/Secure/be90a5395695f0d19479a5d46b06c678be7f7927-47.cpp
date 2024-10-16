MagickExport StringInfo *ConfigureFileToStringInfo(const char *filename)
{
  char
    *string;

  int
    file;

  MagickOffsetType
    offset;

  size_t
    length;

  StringInfo
    *string_info;

  void
    *map;

  assert(filename != (const char *) NULL);
  file=open_utf8(filename,O_RDONLY | O_BINARY,0);
  if (file == -1)
    return((StringInfo *) NULL);
  offset=(MagickOffsetType) lseek(file,0,SEEK_END);
  if ((offset < 0) || (offset != (MagickOffsetType) ((ssize_t) offset)))
    {
      file=close(file)-1;
      return((StringInfo *) NULL);
    }
  length=(size_t) offset;
  string=(char *) NULL;
  if (~length >= (MagickPathExtent-1))
    string=(char *) AcquireQuantumMemory(length+MagickPathExtent,
      sizeof(*string));
  if (string == (char *) NULL)
    {
      file=close(file)-1;
      return((StringInfo *) NULL);
    }
  map=MapBlob(file,ReadMode,0,length);
  if (map != (void *) NULL)
    {
      (void) memcpy(string,map,length);
      (void) UnmapBlob(map,length);
    }
  else
    {
      register size_t
        i;

      ssize_t
        count;

      (void) lseek(file,0,SEEK_SET);
      for (i=0; i < length; i+=count)
      {
        count=read(file,string+i,(size_t) MagickMin(length-i,(size_t)
          SSIZE_MAX));
        if (count <= 0)
          {
            count=0;
            if (errno != EINTR)
              break;
          }
      }
      if (i < length)
        {
          file=close(file)-1;
          string=DestroyString(string);
          return((StringInfo *) NULL);
        }
    }
  string[length]='\0';
  file=close(file)-1;
  string_info=AcquireStringInfoContainer();
  string_info->path=ConstantString(filename);
  string_info->length=length;
  string_info->datum=(unsigned char *) string;
  return(string_info);
}