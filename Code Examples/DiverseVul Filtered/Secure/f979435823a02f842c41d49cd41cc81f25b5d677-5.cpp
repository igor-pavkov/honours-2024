static int append(uint8_t **dst, unsigned *dst_size, const unsigned char *src, unsigned src_size)
{
  *dst = realloc(*dst, *dst_size+src_size);
  if (*dst == NULL)
    return ASN1_MEM_ERROR;
  memcpy(*dst + *dst_size, src, src_size);
  *dst_size += src_size;
  return ASN1_SUCCESS;
}