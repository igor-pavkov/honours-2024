MagickExport MagickOffsetType TellBlob(const Image *image)
{
  BlobInfo
    *magick_restrict blob_info;

  MagickOffsetType
    offset;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(image->blob != (BlobInfo *) NULL);
  assert(image->blob->type != UndefinedStream);
  blob_info=image->blob;
  offset=(-1);
  switch (blob_info->type)
  {
    case UndefinedStream:
    case StandardStream:
      break;
    case FileStream:
    {
      offset=ftell(blob_info->file_info.file);
      break;
    }
    case PipeStream:
      break;
    case ZipStream:
    {
#if defined(MAGICKCORE_ZLIB_DELEGATE)
      offset=(MagickOffsetType) gztell(blob_info->file_info.gzfile);
#endif
      break;
    }
    case BZipStream:
      break;
    case FifoStream:
      break;
    case BlobStream:
    {
      offset=blob_info->offset;
      break;
    }
  }
  return(offset);
}