static size_t my_fread(void *buffer, size_t sz, size_t nmemb, void *userp)
{
  size_t rc;
  struct InStruct *in=(struct InStruct *)userp;

  rc = fread(buffer, sz, nmemb, in->stream);
  return rc;
}