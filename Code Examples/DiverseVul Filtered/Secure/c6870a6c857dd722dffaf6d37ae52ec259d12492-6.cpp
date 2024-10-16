static inline int getd(struct SISTREAM *s, uint32_t *v) {
  if (s->sleft<4) {
    memcpy(s->buff, s->buff + s->smax - s->sleft, s->sleft);
    if ((s->sleft=s->smax=fread(&s->buff[s->sleft], 1, BUFSIZ - s->sleft, s->f) + s->sleft)<4) {
      return 1;
    }
  }
  *v = cli_readint32(&s->buff[s->smax - s->sleft]);
  s->sleft-=4;
  return 0;
}