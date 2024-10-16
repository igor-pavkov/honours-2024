static inline int getsize(struct SISTREAM *s) {
  uint32_t *fsize = &s->fsize[s->level];
  if(getd(s, fsize) || !*fsize || (*fsize)>>31 || (s->level && *fsize > s->fsize[s->level-1] * 2)) return 1;
  /* To handle crafted archives we allow the content to overflow the container but only up to 2 times the container size */
  s->fnext[s->level] = ftell(s->f) - s->sleft + *fsize;
  return 0;
}