static int write_compression_header(blosc2_context* context,
                                    bool extended_header) {
  int32_t compformat;
  int dont_split;
  int dict_training = context->use_dict && (context->dict_cdict == NULL);

  // Set the whole header to zeros so that the reserved values are zeroed
  if (extended_header) {
    memset(context->dest, 0, BLOSC_EXTENDED_HEADER_LENGTH);
  }
  else {
    memset(context->dest, 0, BLOSC_MIN_HEADER_LENGTH);
  }

  /* Write version header for this block */
  context->dest[0] = BLOSC_VERSION_FORMAT;

  /* Write compressor format */
  compformat = -1;
  switch (context->compcode) {
    case BLOSC_BLOSCLZ:
      compformat = BLOSC_BLOSCLZ_FORMAT;
      context->dest[1] = BLOSC_BLOSCLZ_VERSION_FORMAT;
      break;

#if defined(HAVE_LZ4)
    case BLOSC_LZ4:
      compformat = BLOSC_LZ4_FORMAT;
      context->dest[1] = BLOSC_LZ4_VERSION_FORMAT;
      break;
    case BLOSC_LZ4HC:
      compformat = BLOSC_LZ4HC_FORMAT;
      context->dest[1] = BLOSC_LZ4HC_VERSION_FORMAT;
      break;
#endif /*  HAVE_LZ4 */

#if defined(HAVE_LIZARD)
    case BLOSC_LIZARD:
      compformat = BLOSC_LIZARD_FORMAT;
      context->dest[1] = BLOSC_LIZARD_VERSION_FORMAT;
      break;
#endif /*  HAVE_LIZARD */

#if defined(HAVE_SNAPPY)
    case BLOSC_SNAPPY:
      compformat = BLOSC_SNAPPY_FORMAT;
      context->dest[1] = BLOSC_SNAPPY_VERSION_FORMAT;
      break;
#endif /*  HAVE_SNAPPY */

#if defined(HAVE_ZLIB)
    case BLOSC_ZLIB:
      compformat = BLOSC_ZLIB_FORMAT;
      context->dest[1] = BLOSC_ZLIB_VERSION_FORMAT;
      break;
#endif /*  HAVE_ZLIB */

#if defined(HAVE_ZSTD)
    case BLOSC_ZSTD:
      compformat = BLOSC_ZSTD_FORMAT;
      context->dest[1] = BLOSC_ZSTD_VERSION_FORMAT;
      break;
#endif /*  HAVE_ZSTD */

    default: {
      const char* compname;
      compname = clibcode_to_clibname(compformat);
      fprintf(stderr, "Blosc has not been compiled with '%s' ", compname);
      fprintf(stderr, "compression support.  Please use one having it.");
      return -5;    /* signals no compression support */
      break;
    }
  }

  if (context->clevel == 0) {
    /* Compression level 0 means buffer to be memcpy'ed */
    context->header_flags |= (uint8_t)BLOSC_MEMCPYED;
  }

  if (context->sourcesize < BLOSC_MIN_BUFFERSIZE) {
    /* Buffer is too small.  Try memcpy'ing. */
    context->header_flags |= (uint8_t)BLOSC_MEMCPYED;
  }
  bool memcpyed = context->header_flags & (uint8_t)BLOSC_MEMCPYED;

  context->dest[2] = 0;                               /* zeroes flags */
  context->dest[3] = (uint8_t)context->typesize;
  _sw32(context->dest + 4, (int32_t)context->sourcesize);
  _sw32(context->dest + 8, (int32_t)context->blocksize);
  if (extended_header) {
    /* Mark that we are handling an extended header */
    context->header_flags |= (BLOSC_DOSHUFFLE | BLOSC_DOBITSHUFFLE);
    /* Store filter pipeline info at the end of the header */
    uint8_t *filters = context->dest + BLOSC_MIN_HEADER_LENGTH;
    uint8_t *filters_meta = filters + 8;
    for (int i = 0; i < BLOSC2_MAX_FILTERS; i++) {
      filters[i] = context->filters[i];
      filters_meta[i] = context->filters_meta[i];
    }
    uint8_t* blosc2_flags = context->dest + 0x1F;
    *blosc2_flags = 0;    // zeroes flags
    *blosc2_flags |= is_little_endian() ? 0 : BLOSC2_BIGENDIAN;  // endianness
    if (dict_training || memcpyed) {
      context->bstarts = NULL;
      context->output_bytes = BLOSC_EXTENDED_HEADER_LENGTH;
    } else {
      context->bstarts = (int32_t*)(context->dest + BLOSC_EXTENDED_HEADER_LENGTH);
      context->output_bytes = BLOSC_EXTENDED_HEADER_LENGTH +
                              sizeof(int32_t) * context->nblocks;
    }
    if (context->use_dict) {
      *blosc2_flags |= BLOSC2_USEDICT;
    }
  } else {
    // Regular header
    if (memcpyed) {
      context->bstarts = NULL;
      context->output_bytes = BLOSC_MIN_HEADER_LENGTH;
    } else {
      context->bstarts = (int32_t *) (context->dest + BLOSC_MIN_HEADER_LENGTH);
      context->output_bytes = BLOSC_MIN_HEADER_LENGTH +
                              sizeof(int32_t) * context->nblocks;
    }
  }

  // when memcpyed bit is set, there is no point in dealing with others
  if (!memcpyed) {
    if (context->filter_flags & BLOSC_DOSHUFFLE) {
      /* Byte-shuffle is active */
      context->header_flags |= BLOSC_DOSHUFFLE;
    }

    if (context->filter_flags & BLOSC_DOBITSHUFFLE) {
      /* Bit-shuffle is active */
      context->header_flags |= BLOSC_DOBITSHUFFLE;
    }

    if (context->filter_flags & BLOSC_DODELTA) {
      /* Delta is active */
      context->header_flags |= BLOSC_DODELTA;
    }

    dont_split = !split_block(context, context->typesize,
                              context->blocksize, extended_header);
    context->header_flags |= dont_split << 4;  /* dont_split is in bit 4 */
    context->header_flags |= compformat << 5;  /* codec starts at bit 5 */
  }

  // store header flags in dest
  context->dest[2] = context->header_flags;

  return 1;
}