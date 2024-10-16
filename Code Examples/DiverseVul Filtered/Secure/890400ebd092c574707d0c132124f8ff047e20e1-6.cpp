static int readOHDRHeaderMessageDataFill3(struct READER *reader) {
  uint8_t flags;
  uint32_t size;

  flags = (uint8_t)fgetc(reader->fhd);

  if (flags & (1 << 5)) {
    size = (uint32_t)readValue(reader, 4);
    if (fseek(reader->fhd, size, SEEK_CUR) < 0)
      return errno; // LCOV_EXCL_LINE
  }

  return MYSOFA_OK;
}