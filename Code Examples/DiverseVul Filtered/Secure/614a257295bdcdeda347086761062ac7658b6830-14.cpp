MagickExport MagickSizeType GetBlobSize(const Image *image)
{
  BlobInfo
    *magick_restrict blob_info;

  MagickSizeType
    extent;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(image->blob != (BlobInfo *) NULL);
  blob_info=image->blob;
  extent=0;
  switch (blob_info->type)
  {
    case UndefinedStream:
    case StandardStream:
    {
      extent=blob_info->size;
      break;
    }
    case FileStream:
    {
      int
        file_descriptor;

      extent=(MagickSizeType) blob_info->properties.st_size;
      if (extent == 0)
        extent=blob_info->size;
      file_descriptor=fileno(blob_info->file_info.file);
      if (file_descriptor == -1)
        break;
      if (fstat(file_descriptor,&blob_info->properties) == 0)
        extent=(MagickSizeType) blob_info->properties.st_size;
      break;
    }
    case PipeStream:
    {
      extent=blob_info->size;
      break;
    }
    case ZipStream:
    case BZipStream:
    {
      MagickBooleanType
        status;

      status=GetPathAttributes(image->filename,&blob_info->properties);
      if (status != MagickFalse)
        extent=(MagickSizeType) blob_info->properties.st_size;
      break;
    }
    case FifoStream:
      break;
    case BlobStream:
    {
      extent=(MagickSizeType) blob_info->length;
      break;
    }
  }
  return(extent);
}