int blosc_compress_context(blosc2_context* context) {
  int ntbytes = 0;
  blosc_timestamp_t last, current;
  bool memcpyed = context->header_flags & (uint8_t)BLOSC_MEMCPYED;

  blosc_set_timestamp(&last);

  if (!memcpyed) {
    /* Do the actual compression */
    ntbytes = do_job(context);
    if (ntbytes < 0) {
      return -1;
    }
    if (ntbytes == 0) {
      // Try out with a memcpy later on (last chance for fitting src buffer in dest).
      context->header_flags |= (uint8_t)BLOSC_MEMCPYED;
      memcpyed = true;
    }
  }

  if (memcpyed) {
    if (context->sourcesize + BLOSC_MAX_OVERHEAD > context->destsize) {
      /* We are exceeding maximum output size */
      ntbytes = 0;
    }
    else {
      context->output_bytes = BLOSC_MAX_OVERHEAD;
      ntbytes = do_job(context);
      if (ntbytes < 0) {
        return -1;
      }
      // Success!  update the memcpy bit in header
      context->dest[2] = context->header_flags;
      // and clear the memcpy bit in context (for next reuse)
      context->header_flags &= ~(uint8_t)BLOSC_MEMCPYED;
    }
  }

  /* Set the number of compressed bytes in header */
  _sw32(context->dest + 12, ntbytes);

  /* Set the number of bytes in dest buffer (might be useful for btune) */
  context->destsize = ntbytes;

  assert(ntbytes <= context->destsize);

  if (context->btune != NULL) {
    blosc_set_timestamp(&current);
    double ctime = blosc_elapsed_secs(last, current);
    btune_update(context, ctime);
  }

  return ntbytes;
}