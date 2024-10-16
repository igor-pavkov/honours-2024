static curlioerr my_ioctl(CURL *handle, curliocmd cmd, void *userp)
{
  struct InStruct *in=(struct InStruct *)userp;
  (void)handle; /* not used in here */

  switch(cmd) {
  case CURLIOCMD_RESTARTREAD:
    /* mr libcurl kindly asks as to rewind the read data stream to start */
    if(-1 == fseek(in->stream, 0, SEEK_SET))
      /* couldn't rewind, the reason is in errno but errno is just not
         portable enough and we don't actually care that much why we failed. */
      return CURLIOE_FAILRESTART;

    break;

  default: /* ignore unknown commands */
    return CURLIOE_UNKNOWNCMD;
  }
  return CURLIOE_OK;
}