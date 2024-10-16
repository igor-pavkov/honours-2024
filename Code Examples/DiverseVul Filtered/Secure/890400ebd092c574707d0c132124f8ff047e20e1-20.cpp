static int readOHDRHeaderMessageDataFill(struct READER *reader) {

  int version = fgetc(reader->fhd);
  switch (version) {
  case 1:
  case 2:
    return readOHDRHeaderMessageDataFill1or2(reader);
  case 3:
    return readOHDRHeaderMessageDataFill3(reader);
  default:
    // LCOV_EXCL_START
    mylog("object OHDR data storage fill value message must have version 1,2, "
          "or 3 not "
          "%d\n",
          version);
    return MYSOFA_INVALID_FORMAT;
    // LCOV_EXCL_STOP
  }
}