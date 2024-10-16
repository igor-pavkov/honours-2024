MagickExport int EOFBlob(const Image *image)
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
      blob_info->eof=feof(blob_info->file_info.file) != 0 ? MagickTrue :
        MagickFalse;
      break;
    }
    case ZipStream:
    {
#if defined(MAGICKCORE_ZLIB_DELEGATE)
      blob_info->eof=gzeof(blob_info->file_info.gzfile) != 0 ? MagickTrue :
        MagickFalse;
#endif
      break;
    }
    case BZipStream:
    {
#if defined(MAGICKCORE_BZLIB_DELEGATE)
      int
        status;

      status=0;
      (void) BZ2_bzerror(blob_info->file_info.bzfile,&status);
      blob_info->eof=status == BZ_UNEXPECTED_EOF ? MagickTrue : MagickFalse;
#endif
      break;
    }
    case FifoStream:
    {
      blob_info->eof=MagickFalse;
      break;
    }
    case BlobStream:
      break;
  }
  return((int) blob_info->eof);
}