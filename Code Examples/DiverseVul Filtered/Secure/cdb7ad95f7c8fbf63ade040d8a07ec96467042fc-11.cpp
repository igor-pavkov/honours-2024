int FoFiTrueType::seekTable(const char *tag) {
  Guint tagI;
  int i;

  tagI = ((tag[0] & 0xff) << 24) |
         ((tag[1] & 0xff) << 16) |
         ((tag[2] & 0xff) << 8) |
          (tag[3] & 0xff);
  for (i = 0; i < nTables; ++i) {
    if (tables[i].tag == tagI) {
      return i;
    }
  }
  return -1;
}