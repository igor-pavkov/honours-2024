static int readOHDRmessages(struct READER *reader,
                            struct DATAOBJECT *dataobject,
                            uint64_t end_of_messages) {

  FILE *fhd = reader->fhd;
  int err;
  long end;

  while (ftell(fhd) <
         end_of_messages - 4) { /* final gap may has a size of up to 3 */
    uint8_t header_message_type = (uint8_t)fgetc(fhd);
    uint16_t header_message_size = (uint16_t)readValue(reader, 2);
    uint8_t header_message_flags = (uint8_t)fgetc(fhd);
    if ((header_message_flags & ~5) != 0) {
      mylog("OHDR unsupported OHDR message flag %02X\n", header_message_flags);
      return MYSOFA_UNSUPPORTED_FORMAT;
    }

    if ((dataobject->flags & (1 << 2)) != 0)
      /* ignore header_creation_order */
      if (fseek(reader->fhd, 2, SEEK_CUR) < 0)
        return errno;

    mylog(" OHDR message type %2d offset %6lX len %4X\n", header_message_type,
          ftell(fhd), header_message_size);

    end = ftell(fhd) + header_message_size;

    switch (header_message_type) {
    case 0: /* NIL Message */
      if (!!(err = readOHDRHeaderMessageNIL(reader, header_message_size)))
        return err;
      break;
    case 1: /* Dataspace Message */
      if (!!(err = readOHDRHeaderMessageDataspace(reader, &dataobject->ds)))
        return err;
      break;
    case 2: /* Link Info Message */
      if (!!(err = readOHDRHeaderMessageLinkInfo(reader, &dataobject->li)))
        return err;
      break;
    case 3: /* Datatype Message */
      if (!!(err = readOHDRHeaderMessageDatatype(reader, &dataobject->dt)))
        return err;
      break;
    case 4: /* Data Fill Message Old */
      if (!!(err = readOHDRHeaderMessageDataFillOld(reader)))
        return err;
      break;
    case 5: /* Data Fill Message */
      if (!!(err = readOHDRHeaderMessageDataFill(reader)))
        return err;
      break;
    case 8: /* Data Layout Message */
      if (!!(err = readOHDRHeaderMessageDataLayout(reader, dataobject)))
        return err;
      break;
    case 10: /* Group Info Message */
      if (!!(err = readOHDRHeaderMessageGroupInfo(reader, &dataobject->gi)))
        return err;
      break;
    case 11: /* Filter Pipeline Message */
      if (!!(err = readOHDRHeaderMessageFilterPipeline(reader)))
        return err;
      break;
    case 12: /* Attribute Message */
      if (!!(err = readOHDRHeaderMessageAttribute(reader, dataobject)))
        return err;
      break;
    case 16: /* Continue Message */
      if (!!(err = readOHDRHeaderMessageContinue(reader, dataobject)))
        return err;
      break;
    case 21: /* Attribute Info Message */
      if (!!(err = readOHDRHeaderMessageAttributeInfo(reader, &dataobject->ai)))
        return err;
      break;
    default:
      mylog("OHDR unknown header message of type %d\n", header_message_type);

      return MYSOFA_UNSUPPORTED_FORMAT;
    }

    if (ftell(fhd) != end) {
      mylog("OHDR message length mismatch by %ld\n", ftell(fhd) - end);
      return MYSOFA_INTERNAL_ERROR;
    }
  }

  if (fseek(fhd, end_of_messages + 4, SEEK_SET) < 0) /* skip checksum */
    return errno;

  return MYSOFA_OK;
}