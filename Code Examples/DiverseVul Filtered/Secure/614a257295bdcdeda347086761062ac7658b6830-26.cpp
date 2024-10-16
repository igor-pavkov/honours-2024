MagickExport MagickOffsetType SeekBlob(Image *image,
  const MagickOffsetType offset,const int whence)
{
  BlobInfo
    *magick_restrict blob_info;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(image->blob != (BlobInfo *) NULL);
  assert(image->blob->type != UndefinedStream);
  blob_info=image->blob;
  switch (blob_info->type)
  {
    case UndefinedStream:
      break;
    case StandardStream:
    case PipeStream:
      return(-1);
    case FileStream:
    {
      if ((offset < 0) && (whence == SEEK_SET))
        return(-1);
      if (fseek(blob_info->file_info.file,offset,whence) < 0)
        return(-1);
      blob_info->offset=TellBlob(image);
      break;
    }
    case ZipStream:
    {
#if defined(MAGICKCORE_ZLIB_DELEGATE)
      if (gzseek(blob_info->file_info.gzfile,(off_t) offset,whence) < 0)
        return(-1);
#endif
      blob_info->offset=TellBlob(image);
      break;
    }
    case BZipStream:
      return(-1);
    case FifoStream:
      return(-1);
    case BlobStream:
    {
      switch (whence)
      {
        case SEEK_SET:
        default:
        {
          if (offset < 0)
            return(-1);
          blob_info->offset=offset;
          break;
        }
        case SEEK_CUR:
        {
          if (((offset > 0) && (blob_info->offset > (SSIZE_MAX-offset))) ||
              ((offset < 0) && (blob_info->offset < (-SSIZE_MAX-offset))))
            {
              errno=EOVERFLOW;
              return(-1);
            }
          if ((blob_info->offset+offset) < 0)
            return(-1);
          blob_info->offset+=offset;
          break;
        }
        case SEEK_END:
        {
          if (((MagickOffsetType) blob_info->length+offset) < 0)
            return(-1);
          blob_info->offset=blob_info->length+offset;
          break;
        }
      }
      if (blob_info->offset < (MagickOffsetType) ((off_t) blob_info->length))
        {
          blob_info->eof=MagickFalse;
          break;
        }
      if (blob_info->offset >= (MagickOffsetType) ((off_t) blob_info->extent))
        return(-1);
      break;
    }
  }
  return(blob_info->offset);
}