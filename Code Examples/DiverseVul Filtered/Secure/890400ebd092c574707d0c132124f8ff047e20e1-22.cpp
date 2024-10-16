static int readOHDRHeaderMessageAttribute(struct READER *reader,
                                          struct DATAOBJECT *dataobject) {
  int err;

  uint8_t flags, encoding;
  uint16_t name_size, datatype_size, dataspace_size;
  char *name;
  struct DATAOBJECT d;
  struct MYSOFA_ATTRIBUTE *attr;

  UNUSED(encoding);
  UNUSED(datatype_size);
  UNUSED(dataspace_size);

  memset(&d, 0, sizeof(d));

  int version = fgetc(reader->fhd);

  if (version != 1 && version != 3) {
    // LCOV_EXCL_START
    mylog("object OHDR attribute message must have version 1 or 3\n");
    return MYSOFA_INVALID_FORMAT;
    // LCOV_EXCL_STOP
  }

  flags = (uint8_t)fgetc(reader->fhd);

  name_size = (uint16_t)readValue(reader, 2);
  datatype_size = (uint16_t)readValue(reader, 2);
  dataspace_size = (uint16_t)readValue(reader, 2);
  if (version == 3)
    encoding = (uint8_t)fgetc(reader->fhd);

  if (name_size > 0x1000)
    return MYSOFA_NO_MEMORY; // LCOV_EXCL_LINE
  name = malloc(name_size + 1);
  if (!name)
    return MYSOFA_NO_MEMORY; // LCOV_EXCL_LINE
  if (fread(name, 1, name_size, reader->fhd) != name_size) {
    free(name);   // LCOV_EXCL_LINE
    return errno; // LCOV_EXCL_LINE
  }
  if (version == 1 && fseek(reader->fhd, (8 - name_size) & 7, SEEK_CUR) != 0) {
    free(name);   // LCOV_EXCL_LINE
    return errno; // LCOV_EXCL_LINE
  }

  name[name_size] = 0;
  mylog("  attribute name %s %d %d %lX\n", name, datatype_size, dataspace_size,
        ftell(reader->fhd));

  if (version == 3 && (flags & 3)) {
    // LCOV_EXCL_START
    mylog("object OHDR attribute message must not have any flags set\n");
    free(name);
    return MYSOFA_INVALID_FORMAT;
    // LCOV_EXCL_STOP
  }
  err = readOHDRHeaderMessageDatatype(reader, &d.dt);
  if (err) {
    // LCOV_EXCL_START
    mylog("object OHDR attribute message read datatype error\n");
    free(name);
    return MYSOFA_INVALID_FORMAT;
    // LCOV_EXCL_STOP
  }
  if (version == 1) {
    if (fseek(reader->fhd, (8 - datatype_size) & 7, SEEK_CUR) < 0) {
      // LCOV_EXCL_START
      free(name);
      return errno;
      // LCOV_EXCL_STOP
    }
  }

  err = readOHDRHeaderMessageDataspace(reader, &d.ds);
  if (err) {
    // LCOV_EXCL_START
    mylog("object OHDR attribute message read dataspace error\n");
    free(name);
    return MYSOFA_INVALID_FORMAT;
    // LCOV_EXCL_STOP
  }
  if (version == 1) {
    if (fseek(reader->fhd, (8 - dataspace_size) & 7, SEEK_CUR) < 0) {
      // LCOV_EXCL_START
      free(name);
      return errno;
      // LCOV_EXCL_STOP
    }
  }
  err = readData(reader, &d, &d.dt, &d.ds);
  if (err) {
    mylog("object OHDR attribute message read data error\n");
    free(name);
    return MYSOFA_INVALID_FORMAT;
  }

  attr = malloc(sizeof(struct MYSOFA_ATTRIBUTE));
  if (!attr) {
    // LCOV_EXCL_START
    free(name);
    return MYSOFA_NO_MEMORY;
    // LCOV_EXCL_STOP
  }
  attr->name = name;
  attr->value = d.string;
  d.string = NULL;
  attr->next = dataobject->attributes;
  dataobject->attributes = attr;

  dataobjectFree(reader, &d);
  return MYSOFA_OK;
}