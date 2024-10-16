static int readOHDRHeaderMessageFilterPipelineV2(struct READER *reader,
                                                 uint8_t filters) {
  int i, j;
  uint16_t filter_identification_value, flags, number_client_data_values;
  uint32_t client_data;
  uint64_t maximum_compact_value, minimum_dense_value, number_of_entries,
      length_of_entries;

  UNUSED(flags);
  UNUSED(client_data);
  UNUSED(maximum_compact_value);
  UNUSED(minimum_dense_value);
  UNUSED(number_of_entries);
  UNUSED(length_of_entries);

  for (i = 0; i < filters; i++) {
    filter_identification_value = (uint16_t)readValue(reader, 2);
    switch (filter_identification_value) {
    case 1:
    case 2:
      break;
    default:
      // LCOV_EXCL_START
      mylog("object OHDR filter pipeline message contains unsupported filter: "
            "%d\n",
            filter_identification_value);
      return MYSOFA_INVALID_FORMAT;
      // LCOV_EXCL_STOP
    }
    mylog("  filter %d\n", filter_identification_value);
    flags = (uint16_t)readValue(reader, 2);
    number_client_data_values = (uint16_t)readValue(reader, 2);
    if (number_client_data_values > 0x1000)
      return MYSOFA_UNSUPPORTED_FORMAT; // LCOV_EXCL_LINE
    /* no name here */
    for (j = 0; j < number_client_data_values; j++) {
      client_data = readValue(reader, 4);
    }
  }

  return MYSOFA_OK;
}