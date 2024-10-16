DLLEXPORT void tjFree(unsigned char *buf)
{
  if (buf) free(buf);
}