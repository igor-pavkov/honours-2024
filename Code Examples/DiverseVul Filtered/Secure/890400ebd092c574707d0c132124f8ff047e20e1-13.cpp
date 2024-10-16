static int readOHDRHeaderMessageDataFillOld(struct READER *reader) {

  uint32_t size;

  size = (uint32_t)readValue(reader, 4);
  if (fseek(reader->fhd, size, SEEK_CUR) < 0)
    return errno; // LCOV_EXCL_LINE

  return MYSOFA_OK;
}