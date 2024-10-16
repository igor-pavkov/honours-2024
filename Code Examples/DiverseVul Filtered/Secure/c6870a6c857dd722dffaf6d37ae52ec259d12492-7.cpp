static inline int getfield(struct SISTREAM *s, uint32_t *field) {
  int ret;
  if(!(ret = getd(s, field)))
    ret = getsize(s);
  if(!ret) {
    if (*field<T_MAXVALUE)
      cli_dbgmsg("SIS: %d:Got %s(%x) field with size %x\n", s->level, sisfields[*field], *field, s->fsize[s->level]);
    else
      cli_dbgmsg("SIS: %d:Got invalid(%x) field with size %x\n", s->level, *field, s->fsize[s->level]);
  }
  return ret;
}