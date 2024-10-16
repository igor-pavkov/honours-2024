static int readOHDRHeaderMessageDataspace1(struct READER *reader,
                                           struct DATASPACE *ds) {

  int i;

  readValue(reader, 5);

  for (i = 0; i < ds->dimensionality; i++) {
    if (i < 4) {
      ds->dimension_size[i] =
          readValue(reader, reader->superblock.size_of_lengths);
      if (ds->dimension_size[i] > 1000000) {
        mylog("dimension_size is too large\n"); // LCOV_EXCL_LINE
        return MYSOFA_INVALID_FORMAT;           // LCOV_EXCL_LINE
      }
      mylog("   dimension %d %" PRIu64 "\n", i, ds->dimension_size[i]);
    } else
      readValue(reader, reader->superblock.size_of_lengths);
  }

  if (ds->flags & 1) {
    for (i = 0; i < ds->dimensionality; i++) {
      if (i < 4)
        ds->dimension_max_size[i] =
            readValue(reader, reader->superblock.size_of_lengths);
      else
        readValue(reader, reader->superblock.size_of_lengths);
    }
  }

  if (ds->flags & 2) {
    mylog("permutation in OHDR not supported\n"); // LCOV_EXCL_LINE
    return MYSOFA_INVALID_FORMAT;                 // LCOV_EXCL_LINE
  }

  return MYSOFA_OK;
}