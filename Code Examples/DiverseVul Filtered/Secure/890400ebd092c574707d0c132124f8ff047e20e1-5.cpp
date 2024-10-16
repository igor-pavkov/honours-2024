static int readOHDRHeaderMessageAttributeInfo(struct READER *reader,
                                              struct ATTRIBUTEINFO *ai) {

  if (fgetc(reader->fhd) != 0) {
    mylog("object OHDR attribute info message must have version 0\n");
    return MYSOFA_UNSUPPORTED_FORMAT;
  }

  ai->flags = (uint8_t)fgetc(reader->fhd);

  if (ai->flags & 1)
    ai->maximum_creation_index = readValue(reader, 2);

  ai->fractal_heap_address =
      readValue(reader, reader->superblock.size_of_offsets);
  ai->attribute_name_btree =
      readValue(reader, reader->superblock.size_of_offsets);

  if (ai->flags & 2)
    ai->attribute_creation_order_btree =
        readValue(reader, reader->superblock.size_of_offsets);

  return MYSOFA_OK;
}