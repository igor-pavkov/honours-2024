MagickExport unsigned char *FileToBlob(const char *filename,const size_t extent,
  size_t *length,ExceptionInfo *exception)
{
  int
    file;

  MagickBooleanType
    status;

  MagickOffsetType
    offset;

  register size_t
    i;

  ssize_t
    count;

  struct stat
    attributes;

  unsigned char
    *blob;

  void
    *map;

  assert(filename != (const char *) NULL);
  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",filename);
  assert(exception != (ExceptionInfo *) NULL);
  *length=0;
  status=IsRightsAuthorized(PathPolicyDomain,ReadPolicyRights,filename);
  if (status == MagickFalse)
    {
      errno=EPERM;
      (void) ThrowMagickException(exception,GetMagickModule(),PolicyError,
        "NotAuthorized","`%s'",filename);
      return(NULL);
    }
  file=fileno(stdin);
  if (LocaleCompare(filename,"-") != 0)
    {
      status=GetPathAttributes(filename,&attributes);
      if ((status == MagickFalse) || (S_ISDIR(attributes.st_mode) != 0))
        {
          ThrowFileException(exception,BlobError,"UnableToReadBlob",filename);
          return(NULL);
        }
      file=open_utf8(filename,O_RDONLY | O_BINARY,0);
    }
  if (file == -1)
    {
      ThrowFileException(exception,BlobError,"UnableToOpenFile",filename);
      return((unsigned char *) NULL);
    }
  offset=(MagickOffsetType) lseek(file,0,SEEK_END);
  count=0;
  if ((file == fileno(stdin)) || (offset < 0) ||
      (offset != (MagickOffsetType) ((ssize_t) offset)))
    {
      size_t
        quantum;

      struct stat
        file_stats;

      /*
        Stream is not seekable.
      */
      offset=(MagickOffsetType) lseek(file,0,SEEK_SET);
      quantum=(size_t) MagickMaxBufferExtent;
      if ((fstat(file,&file_stats) == 0) && (file_stats.st_size > 0))
        quantum=(size_t) MagickMin(file_stats.st_size,MagickMaxBufferExtent);
      blob=(unsigned char *) AcquireQuantumMemory(quantum,sizeof(*blob));
      for (i=0; blob != (unsigned char *) NULL; i+=count)
      {
        count=read(file,blob+i,quantum);
        if (count <= 0)
          {
            count=0;
            if (errno != EINTR)
              break;
          }
        if (~((size_t) i) < (quantum+1))
          {
            blob=(unsigned char *) RelinquishMagickMemory(blob);
            break;
          }
        blob=(unsigned char *) ResizeQuantumMemory(blob,i+quantum+1,
          sizeof(*blob));
        if ((size_t) (i+count) >= extent)
          break;
      }
      if (LocaleCompare(filename,"-") != 0)
        file=close(file);
      if (blob == (unsigned char *) NULL)
        {
          (void) ThrowMagickException(exception,GetMagickModule(),
            ResourceLimitError,"MemoryAllocationFailed","`%s'",filename);
          return((unsigned char *) NULL);
        }
      if (file == -1)
        {
          blob=(unsigned char *) RelinquishMagickMemory(blob);
          ThrowFileException(exception,BlobError,"UnableToReadBlob",filename);
          return((unsigned char *) NULL);
        }
      *length=(size_t) MagickMin(i+count,extent);
      blob[*length]='\0';
      return(blob);
    }
  *length=(size_t) MagickMin(offset,(MagickOffsetType)
    MagickMin(extent,SSIZE_MAX));
  blob=(unsigned char *) NULL;
  if (~(*length) >= (MaxTextExtent-1))
    blob=(unsigned char *) AcquireQuantumMemory(*length+MaxTextExtent,
      sizeof(*blob));
  if (blob == (unsigned char *) NULL)
    {
      file=close(file);
      (void) ThrowMagickException(exception,GetMagickModule(),
        ResourceLimitError,"MemoryAllocationFailed","`%s'",filename);
      return((unsigned char *) NULL);
    }
  map=MapBlob(file,ReadMode,0,*length);
  if (map != (unsigned char *) NULL)
    {
      (void) memcpy(blob,map,*length);
      (void) UnmapBlob(map,*length);
    }
  else
    {
      (void) lseek(file,0,SEEK_SET);
      for (i=0; i < *length; i+=count)
      {
        count=read(file,blob+i,MagickMin(*length-i,(size_t) SSIZE_MAX));
        if (count <= 0)
          {
            count=0;
            if (errno != EINTR)
              break;
          }
      }
      if (i < *length)
        {
          file=close(file)-1;
          blob=(unsigned char *) RelinquishMagickMemory(blob);
          ThrowFileException(exception,BlobError,"UnableToReadBlob",filename);
          return((unsigned char *) NULL);
        }
    }
  blob[*length]='\0';
  if (LocaleCompare(filename,"-") != 0)
    file=close(file);
  if (file == -1)
    {
      blob=(unsigned char *) RelinquishMagickMemory(blob);
      ThrowFileException(exception,BlobError,"UnableToReadBlob",filename);
    }
  return(blob);
}