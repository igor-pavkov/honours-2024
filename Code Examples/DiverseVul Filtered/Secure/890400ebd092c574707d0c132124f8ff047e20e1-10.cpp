static int readOCHK(struct READER *reader, struct DATAOBJECT *dataobject,
                    uint64_t end) {
  int err;
  char buf[5];

  /* read signature */
  if (fread(buf, 1, 4, reader->fhd) != 4 || strncmp(buf, "OCHK", 4)) {
    mylog("cannot read signature of OCHK\n");
    return MYSOFA_INVALID_FORMAT;
  }
  buf[4] = 0;
  mylog("%08" PRIX64 " %.4s\n", (uint64_t)ftell(reader->fhd) - 4, buf);

  err = readOHDRmessages(reader, dataobject, end - 4); /* subtract checksum */
  if (err) {
    return err;
  }

  return MYSOFA_OK;
}