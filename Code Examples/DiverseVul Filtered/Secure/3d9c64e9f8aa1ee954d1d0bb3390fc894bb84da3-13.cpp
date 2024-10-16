DLLEXPORT int tjDestroy(tjhandle handle)
{
  getinstance(handle);

  if (setjmp(this->jerr.setjmp_buffer)) return -1;
  if (this->init & COMPRESS) jpeg_destroy_compress(cinfo);
  if (this->init & DECOMPRESS) jpeg_destroy_decompress(dinfo);
  free(this);
  return 0;
}