static int readOHDRHeaderMessageLinkInfo(struct READER *reader,
                                         struct LINKINFO *li) {

  if (fgetc(reader->fhd) != 0) {
    mylog(
        "object OHDR link info message must have version 0\n"); // LCOV_EXCL_LINE
    return MYSOFA_UNSUPPORTED_FORMAT; // LCOV_EXCL_LINE
  }

  li->flags = (uint8_t)fgetc(reader->fhd);

  if (li->flags & 1)
    li->maximum_creation_index = readValue(reader, 8);

  li->fractal_heap_address =
      readValue(reader, reader->superblock.size_of_offsets);
  li->address_btree_index =
      readValue(reader, reader->superblock.size_of_offsets);

  if (li->flags & 2)
    li->address_btree_order =
        readValue(reader, reader->superblock.size_of_offsets);

  return MYSOFA_OK;
}