static int readOHDRHeaderMessageContinue(struct READER *reader,
                                         struct DATAOBJECT *dataobject) {

  int err;
  uint64_t offset, length;
  long store;

  offset = readValue(reader, reader->superblock.size_of_offsets);
  length = readValue(reader, reader->superblock.size_of_lengths);
  if (offset > 0x2000000 || length > 0x10000000)
    return MYSOFA_UNSUPPORTED_FORMAT; // LCOV_EXCL_LINE

  mylog(" continue %08" PRIX64 " %08" PRIX64 "\n", offset, length);
  if (reader->recursive_counter >= 25) {
    mylog("recursive problem");
    return MYSOFA_UNSUPPORTED_FORMAT; // LCOV_EXCL_LINE
  } else
    reader->recursive_counter++;

  store = ftell(reader->fhd);

  if (fseek(reader->fhd, offset, SEEK_SET) < 0)
    return errno; // LCOV_EXCL_LINE

  err = readOCHK(reader, dataobject, offset + length);
  if (err)
    return err; // LCOV_EXCL_LINE

  if (store < 0)
    return MYSOFA_READ_ERROR; // LCOV_EXCL_LINE
  if (fseek(reader->fhd, store, SEEK_SET) < 0)
    return errno; // LCOV_EXCL_LINE

  mylog(" continue back\n");
  return MYSOFA_OK;
}