static inline ssize_t WriteBlobStream(Image *image,const size_t length,
  const unsigned char *data)
{
  BlobInfo
    *magick_restrict blob_info;

  MagickSizeType
    extent;

  register unsigned char
    *q;

  assert(image->blob != (BlobInfo *) NULL);
  assert(image->blob->type != UndefinedStream);
  assert(data != (void *) NULL);
  blob_info=image->blob;
  if (blob_info->type != BlobStream)
    return(WriteBlob(image,length,data));
  extent=(MagickSizeType) (blob_info->offset+(MagickOffsetType) length);
  if (extent >= blob_info->extent)
    {
      extent=blob_info->extent+blob_info->quantum+length;
      blob_info->quantum<<=1;
      if (SetBlobExtent(image,extent) == MagickFalse)
        return(0);
    }
  q=blob_info->data+blob_info->offset;
  (void) memcpy(q,data,length);
  blob_info->offset+=length;
  if (blob_info->offset >= (MagickOffsetType) blob_info->length)
    blob_info->length=(size_t) blob_info->offset;
  return((ssize_t) length);
}