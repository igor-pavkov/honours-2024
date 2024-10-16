static int readOHDRHeaderMessageDataFill1or2(struct READER *reader) {

  int spaceAllocationTime = fgetc(reader->fhd);
  int fillValueWriteTime = fgetc(reader->fhd);
  int fillValueDefined = fgetc(reader->fhd);
  if (spaceAllocationTime < 0 || fillValueWriteTime < 0 || fillValueDefined < 0)
    return MYSOFA_READ_ERROR; // LCOV_EXCL_LINE

  if ((spaceAllocationTime & ~1) != 2 || fillValueWriteTime != 2 ||
      (fillValueDefined & ~1) != 0) {
    mylog("spaceAllocationTime %d fillValueWriteTime %d fillValueDefined %d\n",
          spaceAllocationTime, fillValueWriteTime, fillValueDefined);
    return MYSOFA_INVALID_FORMAT; // LCOV_EXCL_LINE
  }
  if (fillValueDefined > 0) {
    uint32_t size = (uint32_t)readValue(reader, 4);
    if (fseek(reader->fhd, size, SEEK_CUR) < 0)
      return errno; // LCOV_EXCL_LINE
  }

  return MYSOFA_OK;
}