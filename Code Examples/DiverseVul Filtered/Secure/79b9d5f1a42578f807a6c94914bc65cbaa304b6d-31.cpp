CURLcode Curl_add_buffer(Curl_send_buffer *in, const void *inptr, size_t size)
{
  char *new_rb;
  size_t new_size;

  if(~size < in->size_used) {
    /* If resulting used size of send buffer would wrap size_t, cleanup
       the whole buffer and return error. Otherwise the required buffer
       size will fit into a single allocatable memory chunk */
    Curl_safefree(in->buffer);
    free(in);
    return CURLE_OUT_OF_MEMORY;
  }

  if(!in->buffer ||
     ((in->size_used + size) > (in->size_max - 1))) {

    /* If current buffer size isn't enough to hold the result, use a
       buffer size that doubles the required size. If this new size
       would wrap size_t, then just use the largest possible one */

    if((size > (size_t)-1/2) || (in->size_used > (size_t)-1/2) ||
       (~(size*2) < (in->size_used*2)))
      new_size = (size_t)-1;
    else
      new_size = (in->size_used+size)*2;

    if(in->buffer)
      /* we have a buffer, enlarge the existing one */
      new_rb = realloc(in->buffer, new_size);
    else
      /* create a new buffer */
      new_rb = malloc(new_size);

    if(!new_rb) {
      /* If we failed, we cleanup the whole buffer and return error */
      Curl_safefree(in->buffer);
      free(in);
      return CURLE_OUT_OF_MEMORY;
    }

    in->buffer = new_rb;
    in->size_max = new_size;
  }
  memcpy(&in->buffer[in->size_used], inptr, size);

  in->size_used += size;

  return CURLE_OK;
}