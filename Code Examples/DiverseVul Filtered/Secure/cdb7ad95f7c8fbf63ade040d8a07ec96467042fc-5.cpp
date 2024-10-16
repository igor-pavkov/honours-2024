GBool FoFiTrueType::getCFFBlock(char **start, int *length) {
  int i;

  if (!openTypeCFF) {
    return gFalse;
  }
  i = seekTable("CFF ");
  if (!checkRegion(tables[i].offset, tables[i].len)) {
    return gFalse;
  }
  *start = (char *)file + tables[i].offset;
  *length = tables[i].len;
  return gTrue;
}