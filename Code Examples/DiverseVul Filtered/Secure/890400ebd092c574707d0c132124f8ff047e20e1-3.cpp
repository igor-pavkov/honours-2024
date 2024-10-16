static int readOHDRHeaderMessageFilterPipeline(struct READER *reader) {
  int filterversion, filters;

  filterversion = fgetc(reader->fhd);
  filters = fgetc(reader->fhd);

  if (filterversion < 0 || filters < 0)
    return MYSOFA_READ_ERROR; // LCOV_EXCL_LINE

  if (filters > 32) {
    // LCOV_EXCL_START
    mylog("object OHDR filter pipeline message has too many filters: %d\n",
          filters);
    return MYSOFA_INVALID_FORMAT;
    // LCOV_EXCL_STOP
  }

  switch (filterversion) {
  case 1:
    return readOHDRHeaderMessageFilterPipelineV1(reader, filters);
  case 2:
    return readOHDRHeaderMessageFilterPipelineV2(reader, filters);
  default:
    // LCOV_EXCL_START
    mylog(
        "object OHDR filter pipeline message must have version 1 or 2 not %d\n",
        filterversion);
    return MYSOFA_INVALID_FORMAT;
    // LCOV_EXCL_STOP
  }
}