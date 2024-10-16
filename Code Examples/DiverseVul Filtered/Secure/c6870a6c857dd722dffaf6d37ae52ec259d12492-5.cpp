static int spamsisnames(FILE *f, uint16_t langs, const char **alangs) {
  uint32_t *lens, *ptrs;
  unsigned int j;

  if (!(lens = cli_malloc(sizeof(uint32_t) * langs * 2))) {
    cli_dbgmsg("SIS: OOM\n");
    return 0;
  }
  if (fread(lens, sizeof(uint32_t) * langs * 2, 1, f)!=1) {
    cli_dbgmsg("SIS: Unable to read lengths and pointers\n");
    free(lens);
    return 1;
  }
  ptrs=&lens[langs];

  for (j=0; j<langs; j++) {
    char *name = getsistring(f,EC32(ptrs[j]),EC32(lens[j]));
    if (name) {
      cli_dbgmsg("\t%s (%s - @%x, len %d)\n", name, alangs[j], EC32(ptrs[j]), EC32(lens[j]));
      free(name);
    }
  }
  free(lens);
  return 1;
}