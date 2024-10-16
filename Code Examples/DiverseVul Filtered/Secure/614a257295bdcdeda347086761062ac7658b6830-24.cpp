MagickExport MagickBooleanType CloseBlob(Image *image)
{
  BlobInfo
    *magick_restrict blob_info;

  int
    status;

  /*
    Close image file.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  blob_info=image->blob;
  if ((blob_info == (BlobInfo *) NULL) || (blob_info->type == UndefinedStream))
    return(MagickTrue);
  status=SyncBlob(image);
  switch (blob_info->type)
  {
    case UndefinedStream:
    case StandardStream:
      break;
    case FileStream:
    case PipeStream:
    {
      if (blob_info->synchronize != MagickFalse)
        status=fsync(fileno(blob_info->file_info.file));
      status=ferror(blob_info->file_info.file);
      break;
    }
    case ZipStream:
    {
#if defined(MAGICKCORE_ZLIB_DELEGATE)
      (void) gzerror(blob_info->file_info.gzfile,&status);
#endif
      break;
    }
    case BZipStream:
    {
#if defined(MAGICKCORE_BZLIB_DELEGATE)
      (void) BZ2_bzerror(blob_info->file_info.bzfile,&status);
#endif
      break;
    }
    case FifoStream:
      break;
    case BlobStream:
    {
      if (blob_info->file_info.file != (FILE *) NULL)
        {
          if (blob_info->synchronize != MagickFalse)
            status=fsync(fileno(blob_info->file_info.file));
          status=ferror(blob_info->file_info.file);
        }
      break;
    }
  }
  blob_info->status=status < 0 ? MagickTrue : MagickFalse;
  blob_info->size=GetBlobSize(image);
  image->extent=blob_info->size;
  blob_info->eof=MagickFalse;
  blob_info->error=0;
  if (blob_info->exempt != MagickFalse)
    {
      blob_info->type=UndefinedStream;
      return(blob_info->status);
    }
  switch (blob_info->type)
  {
    case UndefinedStream:
    case StandardStream:
      break;
    case FileStream:
    {
      if (fileno(blob_info->file_info.file) != -1)
        status=fclose(blob_info->file_info.file);
      break;
    }
    case PipeStream:
    {
#if defined(MAGICKCORE_HAVE_PCLOSE)
      status=pclose(blob_info->file_info.file);
#endif
      break;
    }
    case ZipStream:
    {
#if defined(MAGICKCORE_ZLIB_DELEGATE)
      status=gzclose(blob_info->file_info.gzfile);
#endif
      break;
    }
    case BZipStream:
    {
#if defined(MAGICKCORE_BZLIB_DELEGATE)
      BZ2_bzclose(blob_info->file_info.bzfile);
#endif
      break;
    }
    case FifoStream:
      break;
    case BlobStream:
    {
      if (blob_info->file_info.file != (FILE *) NULL)
        status=fclose(blob_info->file_info.file);
      break;
    }
  }
  (void) DetachBlob(blob_info);
  blob_info->status=status < 0 ? MagickTrue : MagickFalse;
  return(blob_info->status);
}