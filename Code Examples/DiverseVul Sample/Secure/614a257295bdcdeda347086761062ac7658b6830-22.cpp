MagickExport MagickBooleanType IsBlobSeekable(const Image *image)
{
  BlobInfo
    *magick_restrict blob_info;

  assert(image != (const Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  blob_info=image->blob;
  switch (blob_info->type)
  {
    case BlobStream:
      return(MagickTrue);
    case FileStream:
    {
      int
        status;

      if (blob_info->file_info.file == (FILE *) NULL)
        return(MagickFalse);
      status=fseek(blob_info->file_info.file,0,SEEK_CUR);
      return(status == -1 ? MagickFalse : MagickTrue);
    }
    case ZipStream:
    {
#if defined(MAGICKCORE_ZLIB_DELEGATE)
      MagickOffsetType
        offset;

      if (blob_info->file_info.gzfile == (gzFile) NULL)
        return(MagickFalse);
      offset=gzseek(blob_info->file_info.gzfile,0,SEEK_CUR);
      return(offset < 0 ? MagickFalse : MagickTrue);
#else
      break;
#endif
    }
    case UndefinedStream:
    case BZipStream:
    case FifoStream:
    case PipeStream:
    case StandardStream:
      break;
    default:
      break;
  }
  return(MagickFalse);
}