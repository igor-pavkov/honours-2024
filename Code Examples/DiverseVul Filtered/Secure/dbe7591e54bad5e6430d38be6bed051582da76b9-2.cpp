TableLookUp::TableLookUp(int _ntables, bool _dither)
    : ntables(_ntables), dither(_dither) {
  if (ntables < 1) {
    ThrowRDE("Cannot construct 0 tables");
  }
  tables.resize(ntables * TABLE_SIZE, ushort16(0));
}