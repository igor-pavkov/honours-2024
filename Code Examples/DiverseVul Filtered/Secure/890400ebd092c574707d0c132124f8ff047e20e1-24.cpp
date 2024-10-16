static int readOHDRHeaderMessageDataspace2(struct READER *reader,
                                           struct DATASPACE *ds) {

  int i;

  ds->type = (uint8_t)fgetc(reader->fhd);

  for (i = 0; i < ds->dimensionality; i++) {
    if (i < 4) {
      ds->dimension_size[i] =
          readValue(reader, reader->superblock.size_of_lengths);
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

  return MYSOFA_OK;
}