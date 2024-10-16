MagickExport ssize_t ReadBlob(Image *image,const size_t length,
  unsigned char *data)
{
  BlobInfo
    *magick_restrict blob_info;

  int
    c;

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
  assert(data != (void *) NULL);
  blob_info=image->blob;
  count=0;
  q=data;
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
          count=(ssize_t) fread(q,1,length,blob_info->file_info.file);
          break;
        }
        case 4:
        {
          c=getc(blob_info->file_info.file);
          if (c == EOF)
            break;
          *q++=(unsigned char) c;
          count++;
        }
        case 3:
        {
          c=getc(blob_info->file_info.file);
          if (c == EOF)
            break;
          *q++=(unsigned char) c;
          count++;
        }
        case 2:
        {
          c=getc(blob_info->file_info.file);
          if (c == EOF)
            break;
          *q++=(unsigned char) c;
          count++;
        }
        case 1:
        {
          c=getc(blob_info->file_info.file);
          if (c == EOF)
            break;
          *q++=(unsigned char) c;
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
            count=(ssize_t) gzread(blob_info->file_info.gzfile,q+i,
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
          c=gzgetc(blob_info->file_info.gzfile);
          if (c == EOF)
            break;
          *q++=(unsigned char) c;
          count++;
        }
        case 3:
        {
          c=gzgetc(blob_info->file_info.gzfile);
          if (c == EOF)
            break;
          *q++=(unsigned char) c;
          count++;
        }
        case 2:
        {
          c=gzgetc(blob_info->file_info.gzfile);
          if (c == EOF)
            break;
          *q++=(unsigned char) c;
          count++;
        }
        case 1:
        {
          c=gzgetc(blob_info->file_info.gzfile);
          if (c == EOF)
            break;
          *q++=(unsigned char) c;
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
        count=(ssize_t) BZ2_bzread(blob_info->file_info.bzfile,q+i,
          (unsigned int) MagickMin(length-i,MagickMaxBufferExtent));
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
      break;
    case BlobStream:
    {
      register const unsigned char
        *p;

      if (blob_info->offset >= (MagickOffsetType) blob_info->length)
        {
          blob_info->eof=MagickTrue;
          break;
        }
      p=blob_info->data+blob_info->offset;
      count=(ssize_t) MagickMin((MagickOffsetType) length,(MagickOffsetType)
        blob_info->length-blob_info->offset);
      blob_info->offset+=count;
      if (count != (ssize_t) length)
        blob_info->eof=MagickTrue;
      (void) memcpy(q,p,(size_t) count);
      break;
    }
  }
  return(count);
}