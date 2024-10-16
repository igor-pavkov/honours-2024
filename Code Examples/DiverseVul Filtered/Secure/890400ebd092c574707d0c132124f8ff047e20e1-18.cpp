int dataobjectRead(struct READER *reader, struct DATAOBJECT *dataobject,
                   char *name) {
  uint64_t size_of_chunk, end_of_messages;
  int err;
  char buf[5];

  memset(dataobject, 0, sizeof(*dataobject));
  dataobject->address = ftell(reader->fhd);
  dataobject->name = name;

  /* read signature */
  if (fread(buf, 1, 4, reader->fhd) != 4 || strncmp(buf, "OHDR", 4)) {
    mylog("cannot read signature of data object\n");
    return MYSOFA_INVALID_FORMAT;
  }
  buf[4] = 0;
  mylog("%08" PRIX64 " %.4s\n", dataobject->address, buf);

  if (fgetc(reader->fhd) != 2) {
    mylog("object OHDR must have version 2\n");
    return MYSOFA_UNSUPPORTED_FORMAT;
  }

  dataobject->flags = (uint8_t)fgetc(reader->fhd);

  if (dataobject->flags & (1 << 5)) {         /* bit 5 indicated time stamps */
    if (fseek(reader->fhd, 16, SEEK_CUR) < 0) /* skip them */
      return errno;
  }

  if (dataobject->flags & (1 << 4)) { /* bit 4 ? */
    mylog("OHDR: unsupported flags bit 4: %02X\n", dataobject->flags);
    return MYSOFA_UNSUPPORTED_FORMAT;
  }

  size_of_chunk = readValue(reader, 1 << (dataobject->flags & 3));
  if (size_of_chunk > 0x1000000)
    return MYSOFA_UNSUPPORTED_FORMAT;

  end_of_messages = ftell(reader->fhd) + size_of_chunk;

  err = readOHDRmessages(reader, dataobject, end_of_messages);

  if (err) {
    return err;
  }

  if (validAddress(reader, dataobject->ai.attribute_name_btree)) {
    /* not needed
         fseek(reader->fhd, dataobject->ai.attribute_name_btree, SEEK_SET);
         btreeRead(reader, &dataobject->attributes);
    */
  }

  /* parse message attribute info */
  if (validAddress(reader, dataobject->ai.fractal_heap_address)) {
    if (fseek(reader->fhd, dataobject->ai.fractal_heap_address, SEEK_SET) < 0)
      return errno;
    err = fractalheapRead(reader, dataobject, &dataobject->attributes_heap);
    if (err)
      return err;
  }

  /* parse message link info */
  if (validAddress(reader, dataobject->li.fractal_heap_address)) {
    fseek(reader->fhd, dataobject->li.fractal_heap_address, SEEK_SET);
    err = fractalheapRead(reader, dataobject, &dataobject->objects_heap);
    if (err)
      return err;
  }

  if (validAddress(reader, dataobject->li.address_btree_index)) {
    /* not needed
       fseek(reader->fhd, dataobject->li.address_btree_index, SEEK_SET);
       btreeRead(reader, &dataobject->objects);
     */
  }

  dataobject->all = reader->all;
  reader->all = dataobject;

  return MYSOFA_OK;
}