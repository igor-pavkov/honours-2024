static inline int skipthis(struct SISTREAM *s) {
  return skip(s, ALIGN4(s->fsize[s->level]));
}