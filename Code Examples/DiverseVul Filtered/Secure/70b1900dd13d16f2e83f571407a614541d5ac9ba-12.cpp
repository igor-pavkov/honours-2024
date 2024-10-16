void dump(char *timebuf, const char *text,
          FILE *stream, unsigned char *ptr, size_t size,
          trace tracetype, curl_infotype infotype)
{
  size_t i;
  size_t c;

  unsigned int width=0x10;

  if(tracetype == TRACE_ASCII)
    /* without the hex output, we can fit more on screen */
    width = 0x40;

  fprintf(stream, "%s%s, %zd bytes (0x%zx)\n", timebuf, text, size, size);

  for(i=0; i<size; i+= width) {

    fprintf(stream, "%04zx: ", i);

    if(tracetype == TRACE_BIN) {
      /* hex not disabled, show it */
      for(c = 0; c < width; c++)
        if(i+c < size)
          fprintf(stream, "%02x ", ptr[i+c]);
        else
          fputs("   ", stream);
    }

    for(c = 0; (c < width) && (i+c < size); c++) {
      /* check for 0D0A; if found, skip past and start a new line of output */
      if ((tracetype == TRACE_ASCII) &&
          (i+c+1 < size) && ptr[i+c]==0x0D && ptr[i+c+1]==0x0A) {
        i+=(c+2-width);
        break;
      }
#ifdef CURL_DOES_CONVERSIONS
      /* repeat the 0D0A check above but use the host encoding for CRLF */
      if ((tracetype == TRACE_ASCII) &&
          (i+c+1 < size) && ptr[i+c]=='\r' && ptr[i+c+1]=='\n') {
        i+=(c+2-width);
        break;
      }
      /* convert to host encoding and print this character */
      fprintf(stream, "%c", convert_char(infotype, ptr[i+c]));
#else
      (void)infotype;
      fprintf(stream, "%c",
              (ptr[i+c]>=0x20) && (ptr[i+c]<0x80)?ptr[i+c]:UNPRINTABLE_CHAR);
#endif /* CURL_DOES_CONVERSIONS */
      /* check again for 0D0A, to avoid an extra \n if it's at width */
      if ((tracetype == TRACE_ASCII) &&
          (i+c+2 < size) && ptr[i+c+1]==0x0D && ptr[i+c+2]==0x0A) {
        i+=(c+3-width);
        break;
      }
    }
    fputc('\n', stream); /* newline */
  }
  fflush(stream);
}