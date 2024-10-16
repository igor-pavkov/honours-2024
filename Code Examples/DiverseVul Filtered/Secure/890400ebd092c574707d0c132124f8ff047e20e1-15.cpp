static int readOHDRHeaderMessageFilterPipelineV1(struct READER *reader,
                                                 uint8_t filters) {
  int i, j;
  uint16_t filter_identification_value, flags, number_client_data_values,
      namelength;

  if (readValue(reader, 6) != 0) {
    mylog("reserved values not zero\n");
    return MYSOFA_INVALID_FORMAT;
  }

  for (i = 0; i < filters; i++) {
    filter_identification_value = (uint16_t)readValue(reader, 2);
    switch (filter_identification_value) {
    case 1:
    case 2:
      break;
    default:
      // LCOV_EXCL_START
      mylog("object OHDR filter pipeline message contains unsupported filter: "
            "%d %lX\n",
            filter_identification_value, ftell(reader->fhd) - 2);
      return MYSOFA_INVALID_FORMAT;
      // LCOV_EXCL_STOP
    }
    namelength = (uint16_t)readValue(reader, 2);
    flags = (uint16_t)readValue(reader, 2);
    number_client_data_values = (uint16_t)readValue(reader, 2);

    if (namelength > 0)
      if (fseek(reader->fhd, ((namelength - 1) & ~7) + 8, SEEK_CUR) ==
          -1)                     // skip name
        return MYSOFA_READ_ERROR; // LCOV_EXCL_LINE

    mylog("  filter %d namelen %d flags %04X values %d\n",
          filter_identification_value, namelength, flags,
          number_client_data_values);

    if (number_client_data_values > 0x1000)
      return MYSOFA_UNSUPPORTED_FORMAT; // LCOV_EXCL_LINE
    /* no name here */
    for (j = 0; j < number_client_data_values; j++) {
      readValue(reader, 4);
    }
    if ((number_client_data_values & 1) == 1)
      readValue(reader, 4);
  }

  return MYSOFA_OK;
}