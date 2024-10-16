static int SyncBlob(Image *image)
{
  BlobInfo
    *magick_restrict blob_info;

  int
    status;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(image->blob != (BlobInfo *) NULL);
  assert(image->blob->type != UndefinedStream);
  blob_info=image->blob;
  status=0;
  switch (blob_info->type)
  {
    case UndefinedStream:
    case StandardStream:
      break;
    case FileStream:
    case PipeStream:
    {
      status=fflush(blob_info->file_info.file);
      break;
    }
    case ZipStream:
    {
#if defined(MAGICKCORE_ZLIB_DELEGATE)
      status=gzflush(blob_info->file_info.gzfile,Z_SYNC_FLUSH);
#endif
      break;
    }
    case BZipStream:
    {
#if defined(MAGICKCORE_BZLIB_DELEGATE)
      status=BZ2_bzflush(blob_info->file_info.bzfile);
#endif
      break;
    }
    case FifoStream:
      break;
    case BlobStream:
      break;
  }
  return(status);
}