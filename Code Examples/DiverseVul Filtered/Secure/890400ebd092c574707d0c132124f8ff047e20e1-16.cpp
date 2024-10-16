static int readOHDRHeaderMessageGroupInfo(struct READER *reader,
                                          struct GROUPINFO *gi) {

  if (fgetc(reader->fhd) != 0) {
    // LCOV_EXCL_START
    mylog("object OHDR group info message must have version 0\n");
    return MYSOFA_UNSUPPORTED_FORMAT;
    // LCOV_EXCL_STOP
  }

  gi->flags = (uint8_t)fgetc(reader->fhd);

  if (gi->flags & 1) {
    gi->maximum_compact_value = (uint16_t)readValue(reader, 2);
    gi->minimum_dense_value = (uint16_t)readValue(reader, 2);
  }

  if (gi->flags & 2) {
    gi->number_of_entries = (uint16_t)readValue(reader, 2);
    gi->length_of_entries = (uint16_t)readValue(reader, 2);
  }
  return MYSOFA_OK;
}