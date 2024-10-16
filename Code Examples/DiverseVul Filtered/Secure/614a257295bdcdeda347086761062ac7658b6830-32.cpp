MagickExport int ErrorBlob(const Image *image)
{
  BlobInfo
    *magick_restrict blob_info;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(image->blob != (BlobInfo *) NULL);
  assert(image->blob->type != UndefinedStream);
  blob_info=image->blob;
  switch (blob_info->type)
  {
    case UndefinedStream:
    case StandardStream:
      break;
    case FileStream:
    case PipeStream:
    {
      blob_info->error=ferror(blob_info->file_info.file);
      break;
    }
    case ZipStream:
    {
#if defined(MAGICKCORE_ZLIB_DELEGATE)
      (void) gzerror(blob_info->file_info.gzfile,&blob_info->error);
#endif
      break;
    }
    case BZipStream:
    {
#if defined(MAGICKCORE_BZLIB_DELEGATE)
      (void) BZ2_bzerror(blob_info->file_info.bzfile,&blob_info->error);
#endif
      break;
    }
    case FifoStream:
    {
      blob_info->error=0;
      break;
    }
    case BlobStream:
      break;
  }
  return(blob_info->error);
}