static OPJ_OFF_T JP2SkipHandler(OPJ_OFF_T offset,void *context)
{
  Image
    *image;

  image=(Image *) context;
  return(SeekBlob(image,offset,SEEK_CUR) < 0 ? -1 : offset);
}