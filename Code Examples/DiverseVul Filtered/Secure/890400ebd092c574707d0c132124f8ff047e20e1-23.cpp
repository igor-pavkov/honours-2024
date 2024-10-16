static struct DATAOBJECT *findDataobject(struct READER *reader,
                                         uint64_t address) {
  struct DATAOBJECT *p = reader->all;
  while (p && p->address != address)
    p = p->all;

  return p;
}