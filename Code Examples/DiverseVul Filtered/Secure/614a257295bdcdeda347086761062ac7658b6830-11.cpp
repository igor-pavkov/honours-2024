MagickExport MagickBooleanType SetBlobExtent(Image *image,
  const MagickSizeType extent)
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
      return(MagickFalse);
    case FileStream:
    {
      MagickOffsetType
        offset;

      ssize_t
        count;

      if (extent != (MagickSizeType) ((off_t) extent))
        return(MagickFalse);
      offset=SeekBlob(image,0,SEEK_END);
      if (offset < 0)
        return(MagickFalse);
      if ((MagickSizeType) offset >= extent)
        break;
      offset=SeekBlob(image,(MagickOffsetType) extent-1,SEEK_SET);
      if (offset < 0)
        break;
      count=(ssize_t) fwrite((const unsigned char *) "",1,1,
        blob_info->file_info.file);
#if defined(MAGICKCORE_HAVE_POSIX_FALLOCATE)
      if (blob_info->synchronize != MagickFalse)
        {
          int
            file;

          file=fileno(blob_info->file_info.file);
          if ((file == -1) || (offset < 0))
            return(MagickFalse);
          (void) posix_fallocate(file,offset,extent-offset);
        }
#endif
      offset=SeekBlob(image,offset,SEEK_SET);
      if (count != 1)
        return(MagickFalse);
      break;
    }
    case PipeStream:
    case ZipStream:
      return(MagickFalse);
    case BZipStream:
      return(MagickFalse);
    case FifoStream:
      return(MagickFalse);
    case BlobStream:
    {
      if (extent != (MagickSizeType) ((size_t) extent))
        return(MagickFalse);
      if (blob_info->mapped != MagickFalse)
        {
          MagickOffsetType
            offset;

          ssize_t
            count;

          (void) UnmapBlob(blob_info->data,blob_info->length);
          RelinquishMagickResource(MapResource,blob_info->length);
          if (extent != (MagickSizeType) ((off_t) extent))
            return(MagickFalse);
          offset=SeekBlob(image,0,SEEK_END);
          if (offset < 0)
            return(MagickFalse);
          if ((MagickSizeType) offset >= extent)
            break;
          offset=SeekBlob(image,(MagickOffsetType) extent-1,SEEK_SET);
          count=(ssize_t) fwrite((const unsigned char *) "",1,1,
            blob_info->file_info.file);
#if defined(MAGICKCORE_HAVE_POSIX_FALLOCATE)
          if (blob_info->synchronize != MagickFalse)
            {
              int
                file;

              file=fileno(blob_info->file_info.file);
              if ((file == -1) || (offset < 0))
                return(MagickFalse);
              (void) posix_fallocate(file,offset,extent-offset);
            }
#endif
          offset=SeekBlob(image,offset,SEEK_SET);
          if (count != 1)
            return(MagickFalse);
          (void) AcquireMagickResource(MapResource,extent);
          blob_info->data=(unsigned char*) MapBlob(fileno(
            blob_info->file_info.file),WriteMode,0,(size_t) extent);
          blob_info->extent=(size_t) extent;
          blob_info->length=(size_t) extent;
          (void) SyncBlob(image);
          break;
        }
      blob_info->extent=(size_t) extent;
      blob_info->data=(unsigned char *) ResizeQuantumMemory(blob_info->data,
        blob_info->extent+1,sizeof(*blob_info->data));
      (void) SyncBlob(image);
      if (blob_info->data == (unsigned char *) NULL)
        {
          (void) DetachBlob(blob_info);
          return(MagickFalse);
        }
      break;
    }
  }
  return(MagickTrue);
}