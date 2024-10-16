static tsize_t TIFFWriteBlob(thandle_t image,tdata_t data,tsize_t size)
{
  tsize_t
    count;

  count=(tsize_t) WriteBlob((Image *) image,(size_t) size,
    (unsigned char *) data);
  return(count);
}