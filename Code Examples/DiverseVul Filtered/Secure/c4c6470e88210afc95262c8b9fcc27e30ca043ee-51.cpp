void blosc_cbuffer_sizes(const void* cbuffer, size_t* nbytes,
                         size_t* cbytes, size_t* blocksize) {
  uint8_t* _src = (uint8_t*)(cbuffer);    /* current pos for source buffer */
  uint8_t version = _src[0];                        /* blosc format version */
  if (version > BLOSC_VERSION_FORMAT) {
    /* Version from future */
    *nbytes = *blocksize = *cbytes = 0;
    return;
  }

  /* Read the interesting values */
  *nbytes = (size_t)sw32_(_src + 4);       /* uncompressed buffer size */
  *blocksize = (size_t)sw32_(_src + 8);    /* block size */
  *cbytes = (size_t)sw32_(_src + 12);      /* compressed buffer size */
}