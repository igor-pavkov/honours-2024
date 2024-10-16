static int readOHDRHeaderMessageDataspace(struct READER *reader,
                                          struct DATASPACE *ds) {

  int version = fgetc(reader->fhd);

  ds->dimensionality = (uint8_t)fgetc(reader->fhd);
  if (ds->dimensionality > 4) {
    mylog("dimensionality must be lower than 5\n"); // LCOV_EXCL_LINE
    return MYSOFA_INVALID_FORMAT;                   // LCOV_EXCL_LINE
  }

  ds->flags = (uint8_t)fgetc(reader->fhd);

  switch (version) {
  case 1:
    return readOHDRHeaderMessageDataspace1(reader, ds);
  case 2:
    return readOHDRHeaderMessageDataspace2(reader, ds);
  default:
    // LCOV_EXCL_START
    mylog("object OHDR dataspace message must have version 1 or 2 but is %X at "
          "%lX\n",
          version, ftell(reader->fhd) - 1);
    return MYSOFA_INVALID_FORMAT;
    // LCOV_EXCL_STOP
  }
}