void CLASS tiff_get(unsigned base, unsigned *tag, unsigned *type, unsigned *len, unsigned *save)
{
#ifdef LIBRAW_IOSPACE_CHECK
  INT64 pos = ftell(ifp);
  INT64 fsize = ifp->size();
  if(fsize < 12 || (fsize-pos) < 12)
     throw LIBRAW_EXCEPTION_IO_EOF;
#endif
  *tag = get2();
  *type = get2();
  *len = get4();
  *save = ftell(ifp) + 4;
  if (*len * ("11124811248484"[*type < 14 ? *type : 0] - '0') > 4)
    fseek(ifp, get4() + base, SEEK_SET);
}