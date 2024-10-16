static size_t my_fwrite(void *buffer, size_t sz, size_t nmemb, void *stream)
{
  size_t rc;
  struct OutStruct *out=(struct OutStruct *)stream;
  struct Configurable *config = out->config;

  if(!out->stream) {
    /* open file for writing */
    out->stream=fopen(out->filename, "wb");
    if(!out->stream) {
      warnf(config, "Failed to create the file %s\n", out->filename);
      /*
       * Once that libcurl has called back my_fwrite() the returned value
       * is checked against the amount that was intended to be written, if
       * it does not match then it fails with CURLE_WRITE_ERROR. So at this
       * point returning a value different from sz*nmemb indicates failure.
       */
      rc = (0 == (sz * nmemb)) ? 1 : 0;
      return rc; /* failure */
    }
  }

  rc = fwrite(buffer, sz, nmemb, out->stream);

  if((sz * nmemb) == rc) {
    /* we added this amount of data to the output */
    out->bytes += (sz * nmemb);
  }

  if(config->nobuffer)
    /* disable output buffering */
    fflush(out->stream);

  return rc;
}