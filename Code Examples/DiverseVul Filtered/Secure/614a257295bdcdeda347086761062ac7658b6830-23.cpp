MagickExport MagickBooleanType FileToImage(Image *image,const char *filename)
{
  int
    file;

  MagickBooleanType
    status;

  size_t
    length,
    quantum;

  ssize_t
    count;

  struct stat
    file_stats;

  unsigned char
    *blob;

  assert(image != (const Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  assert(filename != (const char *) NULL);
  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",filename);
  status=IsRightsAuthorized(PathPolicyDomain,WritePolicyRights,filename);
  if (status == MagickFalse)
    {
      errno=EPERM;
      (void) ThrowMagickException(&image->exception,GetMagickModule(),
        PolicyError,"NotAuthorized","`%s'",filename);
      return(MagickFalse);
    }
  file=fileno(stdin);
  if (LocaleCompare(filename,"-") != 0)
    file=open_utf8(filename,O_RDONLY | O_BINARY,0);
  if (file == -1)
    {
      ThrowFileException(&image->exception,BlobError,"UnableToOpenBlob",
        filename);
      return(MagickFalse);
    }
  quantum=(size_t) MagickMaxBufferExtent;
  if ((fstat(file,&file_stats) == 0) && (file_stats.st_size > 0))
    quantum=(size_t) MagickMin(file_stats.st_size,MagickMaxBufferExtent);
  blob=(unsigned char *) AcquireQuantumMemory(quantum,sizeof(*blob));
  if (blob == (unsigned char *) NULL)
    {
      file=close(file);
      ThrowFileException(&image->exception,ResourceLimitError,
        "MemoryAllocationFailed",filename);
      return(MagickFalse);
    }
  for ( ; ; )
  {
    count=read(file,blob,quantum);
    if (count <= 0)
      {
        count=0;
        if (errno != EINTR)
          break;
      }
    length=(size_t) count;
    count=WriteBlobStream(image,length,blob);
    if (count != (ssize_t) length)
      {
        ThrowFileException(&image->exception,BlobError,"UnableToWriteBlob",
          filename);
        break;
      }
  }
  file=close(file);
  if (file == -1)
    ThrowFileException(&image->exception,BlobError,"UnableToWriteBlob",
      filename);
  blob=(unsigned char *) RelinquishMagickMemory(blob);
  return(MagickTrue);
}