static inline int skip(struct SISTREAM *s, uint32_t size) {
  long seekto;
  cli_dbgmsg("SIS: skipping %x\n", size);
  if (s->sleft>=size) s->sleft-=size;
  else {
    seekto = size - s->sleft;
    if (seekto<0) /* in case sizeof(long)==sizeof(uint32_t) */
      return 1;
    fseek(s->f, seekto, SEEK_CUR);
    /*     s->sleft = s->smax = fread(s->buff,1,BUFSIZ,s->f); */
    s->sleft = s->smax = 0;
  }
  return 0;
}