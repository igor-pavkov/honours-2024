ushort16* TableLookUp::getTable(int n) {
  if (n > ntables) {
    ThrowRDE("Table lookup with number greater than number of tables.");
  }
  return &tables[n * TABLE_SIZE];
}