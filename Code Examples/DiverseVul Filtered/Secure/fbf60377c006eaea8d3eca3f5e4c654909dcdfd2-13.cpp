static int stread(char *buf, size_t len, LibRaw_abstract_datastream *fp)
{
  if(len>0)
  {
    int r = fp->read(buf, len, 1);
    buf[len - 1] = 0;
    return r;
  }
  else
    return 0;
}