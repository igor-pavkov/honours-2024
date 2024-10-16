static int readOHDRHeaderMessageDatatype(struct READER *reader,
                                         struct DATATYPE *dt) {

  int i, j, c, err;
  char *buffer;
  struct DATATYPE dt2;

  dt->class_and_version = (uint8_t)fgetc(reader->fhd);
  if ((dt->class_and_version & 0xf0) != 0x10 &&
      (dt->class_and_version & 0xf0) != 0x30) {
    // LCOV_EXCL_START
    mylog("object OHDR datatype message must have version 1 not %d at %lX\n",
          dt->class_and_version >> 4, ftell(reader->fhd) - 1);
    return MYSOFA_UNSUPPORTED_FORMAT;
    // LCOV_EXCL_STOP
  }

  dt->class_bit_field = (uint32_t)readValue(reader, 3);
  dt->size = (uint32_t)readValue(reader, 4);
  if (dt->size > 64)
    return MYSOFA_UNSUPPORTED_FORMAT; // LCOV_EXCL_LINE

  switch (dt->class_and_version & 0xf) {
  case 0: /* int */
    dt->u.i.bit_offset = readValue(reader, 2);
    dt->u.i.bit_precision = readValue(reader, 2);
    mylog("    INT bit %d %d %d %d\n", dt->u.i.bit_offset,
          dt->u.i.bit_precision, dt->class_and_version >> 4, dt->size);
    break;

  case 1: /* float */
    dt->u.f.bit_offset = (uint16_t)readValue(reader, 2);
    dt->u.f.bit_precision = (uint16_t)readValue(reader, 2);
    dt->u.f.exponent_location = (uint8_t)fgetc(reader->fhd);
    dt->u.f.exponent_size = (uint8_t)fgetc(reader->fhd);
    dt->u.f.mantissa_location = (uint8_t)fgetc(reader->fhd);
    dt->u.f.mantissa_size = (uint8_t)fgetc(reader->fhd);
    dt->u.f.exponent_bias = (uint32_t)readValue(reader, 4);

    mylog("    FLOAT bit %d %d exponent %d %d MANTISSA %d %d OFFSET %d\n",
          dt->u.f.bit_offset, dt->u.f.bit_precision, dt->u.f.exponent_location,
          dt->u.f.exponent_size, dt->u.f.mantissa_location,
          dt->u.f.mantissa_size, dt->u.f.exponent_bias);

    /* FLOAT bit 0 32 exponent 23 8 MANTISSA 0 23 OFFSET 127
     FLOAT bit 0 64 exponent 52 11 MANTISSA 0 52 OFFSET 1023 */

    if (dt->u.f.bit_offset != 0 || dt->u.f.mantissa_location != 0 ||
        (dt->u.f.bit_precision != 32 && dt->u.f.bit_precision != 64) ||
        (dt->u.f.bit_precision == 32 &&
         (dt->u.f.exponent_location != 23 || dt->u.f.exponent_size != 8 ||
          dt->u.f.mantissa_size != 23 || dt->u.f.exponent_bias != 127)) ||
        (dt->u.f.bit_precision == 64 &&
         (dt->u.f.exponent_location != 52 || dt->u.f.exponent_size != 11 ||
          dt->u.f.mantissa_size != 52 || dt->u.f.exponent_bias != 1023)))
      return MYSOFA_UNSUPPORTED_FORMAT; // LCOV_EXCL_LINE
    break;

  case 3: /* string */
    mylog("    STRING %d %02X\n", dt->size, dt->class_bit_field);
    break;

  case 6:
    mylog("    COMPOUND %d %02X\n", dt->size, dt->class_bit_field);
    switch (dt->class_and_version >> 4) {
    case 3:
      for (i = 0; i < (dt->class_bit_field & 0xffff); i++) {
        int maxsize = 0x1000;
        buffer = malloc(maxsize);
        if (!buffer)
          return MYSOFA_NO_MEMORY;
        for (j = 0; j < maxsize - 1; j++) {
          c = fgetc(reader->fhd);
          if (c < 0) {
            free(buffer);
            return MYSOFA_READ_ERROR;
          }
          buffer[j] = c;
          if (c == 0)
            break;
        }
        buffer[j] = 0;

        for (j = 0, c = 0; (dt->size >> (8 * j)) > 0; j++) {
          c |= fgetc(reader->fhd) << (8 * j);
        }

        mylog("   COMPOUND %s offset %d\n", buffer, c);

        /* not needed until the data is stored somewhere permanently
         p = realloc(buffer, j);
         if (!p) {
         free(buffer);
         return errno;
         }
         buffer = p;
         */
        free(buffer);

        err = readOHDRHeaderMessageDatatype(reader, &dt2);
        if (err)
          return err; // LCOV_EXCL_LINE
      }
      break;

    case 1:
      for (i = 0; i < (dt->class_bit_field & 0xffff); i++) {
        char name[256];
        int res;
        for (j = 0;; j++) {
          if (j == sizeof(name))
            return MYSOFA_INVALID_FORMAT; // LCOV_EXCL_LINE
          res = fgetc(reader->fhd);
          if (res < 0)
            return MYSOFA_READ_ERROR; // LCOV_EXCL_LINE
          name[j] = res;
          if (name[j] == 0)
            break;
        }
        if (fseek(reader->fhd, (7 - j) & 7, SEEK_CUR))
          return MYSOFA_READ_ERROR; // LCOV_EXCL_LINE

        c = readValue(reader, 4);
        int dimension = fgetc(reader->fhd);
        if (dimension != 0) {
          mylog("COMPOUND v1 with dimension not supported");
          return MYSOFA_INVALID_FORMAT; // LCOV_EXCL_LINE
        }

        // ignore the following fields
        if (fseek(reader->fhd, 3 + 4 + 4 + 4 * 4, SEEK_CUR))
          return MYSOFA_READ_ERROR; // LCOV_EXCL_LINE

        mylog("  COMPOUND %s %d %d %lX\n", name, c, dimension,
              ftell(reader->fhd));
        err = readOHDRHeaderMessageDatatype(reader, &dt2);
        if (err)
          return err; // LCOV_EXCL_LINE
      }
      break;
    default:
      // LCOV_EXCL_START
      mylog("object OHDR datatype message must have version 1 or 3 not %d\n",
            dt->class_and_version >> 4);
      return MYSOFA_INVALID_FORMAT;
      // LCOV_EXCL_STOP
    }
    break;
  case 7: /* reference */
    mylog("    REFERENCE %d %02X\n", dt->size, dt->class_bit_field);
    break;

  case 9: /* list */
    dt->list = dt->size;
    mylog("  LIST %d\n", dt->size);
    err = readOHDRHeaderMessageDatatype(reader, dt);
    if (err)
      return err; // LCOV_EXCL_LINE
    break;

  default:
    // LCOV_EXCL_START
    mylog("object OHDR datatype message has unknown variable type %d\n",
          dt->class_and_version & 0xf);
    return MYSOFA_UNSUPPORTED_FORMAT;
    // LCOV_EXCL_STOP
  }
  return MYSOFA_OK;
}