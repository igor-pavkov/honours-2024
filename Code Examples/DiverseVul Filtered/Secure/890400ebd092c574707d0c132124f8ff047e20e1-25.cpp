int readDataVar(struct READER *reader, struct DATAOBJECT *data,
                struct DATATYPE *dt, struct DATASPACE *ds) {

  char *buffer, number[20];
  uint64_t reference, gcol = 0, dataobject;
  int err;
  struct DATAOBJECT *referenceData;

  if (dt->list) {
    if (dt->list - dt->size == 8) {
      readValue(reader, 4); /* TODO unknown? */
      gcol = readValue(reader, 4);
    } else {
      gcol = readValue(reader, dt->list - dt->size);
    }
    mylog("    GCOL %d %8" PRIX64 " %8lX\n", dt->list - dt->size, gcol,
          ftell(reader->fhd));
    /*		fseek(reader->fhd, dt->list - dt->size, SEEK_CUR); TODO:
     * TODO: missing part in specification */
  }

  switch (dt->class_and_version & 0xf) {
  case 0:
    mylog("FIXED POINT todo %lX %d\n", ftell(reader->fhd), dt->size);
    if (fseek(reader->fhd, dt->size, SEEK_CUR))
      return errno; // LCOV_EXCL_LINE
    break;

  case 3:
    buffer = malloc(dt->size + 1);
    if (buffer == NULL) {
      return MYSOFA_NO_MEMORY; // LCOV_EXCL_LINE
    }
    if (fread(buffer, 1, dt->size, reader->fhd) != dt->size) {
      free(buffer);             // LCOV_EXCL_LINE
      return MYSOFA_READ_ERROR; // LCOV_EXCL_LINE
    }
    buffer[dt->size] = 0;
    mylog("STRING %s\n", buffer);
    data->string = buffer;
    break;

    /*
     * 000036e3   67 0e 00 00 00  00 00 00 00 00 00 00 00  |...g............|
     000036f0  00 00 00
     */
  case 6:
    /* TODO unclear spec */
    mylog("COMPONENT todo %lX %d\n", ftell(reader->fhd), dt->size);
    if (fseek(reader->fhd, dt->size, SEEK_CUR))
      return errno; // LCOV_EXCL_LINE
    break;

  case 7:
    readValue(reader, 4); /* TODO unclear reference */
    reference = readValue(reader, dt->size - 4);
    mylog(" REFERENCE size %d %" PRIX64 "\n", dt->size, reference);
    if (!!(err = gcolRead(reader, gcol, reference, &dataobject))) {
      return MYSOFA_OK; /* ignore error. TODO: why?
       return err; */
    }
    referenceData = findDataobject(reader, dataobject);
    if (referenceData)
      buffer = referenceData->name;
    else {
      sprintf(number, "REF%08lX", (long unsigned int)reference);
      buffer = number;
    }
    mylog("    REFERENCE %" PRIX64 " %" PRIX64 " %s\n", reference, dataobject,
          buffer);
    /*		if(!referenceData) { TODO?
     return MYSOFA_UNSUPPORTED_FORMAT;
     } */
    if (data->string) {
      data->string =
          realloc(data->string, strlen(data->string) + strlen(buffer) + 2);
      if (!data->string)
        return MYSOFA_NO_MEMORY;
      strcat(data->string, ",");
      strcat(data->string, buffer);
    } else {
      data->string = mysofa_strdup(buffer);
    }
    break;

  default:
    // LCOV_EXCL_START
    mylog("data reader unknown type %d\n", dt->class_and_version & 0xf);
    return MYSOFA_INTERNAL_ERROR;
    // LCOV_EXCL_STOP
  }
  return MYSOFA_OK;
}