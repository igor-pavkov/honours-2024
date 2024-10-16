static int readOHDRHeaderMessageNIL(struct READER *reader, int length) {

  if (fseek(reader->fhd, length, SEEK_CUR) < 0)
    return errno; // LCOV_EXCL_LINE

  return MYSOFA_OK;
}