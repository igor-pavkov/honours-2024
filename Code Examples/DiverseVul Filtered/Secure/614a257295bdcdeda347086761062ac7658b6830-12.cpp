MagickExport ssize_t WriteBlob(Image *image,const size_t length,
  const unsigned char *data)
{
  BlobInfo
    *magick_restrict blob_info;

  int
    c;

  register const unsigned char
    *p;

  register unsigned char
    *q;

  ssize_t
    count;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  assert(image->blob != (BlobInfo *) NULL);
  assert(image->blob->type != UndefinedStream);
  if (length == 0)
    return(0);
  assert(data != (const unsigned char *) NULL);
  blob_info=image->blob;
  count=0;
  p=(const unsigned char *) data;
  q=(unsigned char *) data;
  switch (blob_info->type)
  {
    case UndefinedStream:
      break;
    case StandardStream:
    case FileStream:
    case PipeStream:
    {
      switch (length)
      {
        default:
        {
          count=(ssize_t) fwrite((const char *) data,1,length,
            blob_info->file_info.file);
          break;
        }
        case 4:
        {
          c=putc((int) *p++,blob_info->file_info.file);
          if (c == EOF)
            break;
          count++;
        }
        case 3:
        {
          c=putc((int) *p++,blob_info->file_info.file);
          if (c == EOF)
            break;
          count++;
        }
        case 2:
        {
          c=putc((int) *p++,blob_info->file_info.file);
          if (c == EOF)
            break;
          count++;
        }
        case 1:
        {
          c=putc((int) *p++,blob_info->file_info.file);
          if (c == EOF)
            break;
          count++;
        }
        case 0:
          break;
      }
      break;
    }
    case ZipStream:
    {
#if defined(MAGICKCORE_ZLIB_DELEGATE)
      switch (length)
      {
        default:
        {
          register ssize_t
            i;

          for (i=0; i < (ssize_t) length; i+=count)
          {
            count=(ssize_t) gzwrite(blob_info->file_info.gzfile,q+i,
              (unsigned int) MagickMin(length-i,MagickMaxBufferExtent));
            if (count <= 0)
              {
                count=0;
                if (errno != EINTR)
                  break;
              }
          }
          count=i;
          break;
        }
        case 4:
        {
          c=gzputc(blob_info->file_info.gzfile,(int) *p++);
          if (c == EOF)
            break;
          count++;
        }
        case 3:
        {
          c=gzputc(blob_info->file_info.gzfile,(int) *p++);
          if (c == EOF)
            break;
          count++;
        }
        case 2:
        {
          c=gzputc(blob_info->file_info.gzfile,(int) *p++);
          if (c == EOF)
            break;
          count++;
        }
        case 1:
        {
          c=gzputc(blob_info->file_info.gzfile,(int) *p++);
          if (c == EOF)
            break;
          count++;
        }
        case 0:
          break;
      }
#endif
      break;
    }
    case BZipStream:
    {
#if defined(MAGICKCORE_BZLIB_DELEGATE)
      register ssize_t
        i;

      for (i=0; i < (ssize_t) length; i+=count)
      {
        count=(ssize_t) BZ2_bzwrite(blob_info->file_info.bzfile,q+i,
          (int) MagickMin(length-i,MagickMaxBufferExtent));
        if (count <= 0)
          {
            count=0;
            if (errno != EINTR)
              break;
          }
      }
      count=i;
#endif
      break;
    }
    case FifoStream:
    {
      count=(ssize_t) blob_info->stream(image,data,length);
      break;
    }
    case BlobStream:
    {
      if ((blob_info->offset+(MagickOffsetType) length) >=
          (MagickOffsetType) blob_info->extent)
        {
          if (blob_info->mapped != MagickFalse)
            return(0);
          blob_info->extent+=length+blob_info->quantum;
          blob_info->quantum<<=1;
          blob_info->data=(unsigned char *) ResizeQuantumMemory(
            blob_info->data,blob_info->extent+1,sizeof(*blob_info->data));
          (void) SyncBlob(image);
          if (blob_info->data == (unsigned char *) NULL)
            {
              (void) DetachBlob(blob_info);
              return(0);
            }
        }
      q=blob_info->data+blob_info->offset;
      (void) memcpy(q,p,length);
      blob_info->offset+=length;
      if (blob_info->offset >= (MagickOffsetType) blob_info->length)
        blob_info->length=(size_t) blob_info->offset;
      count=(ssize_t) length;
    }
  }
  return(count);
}