void blosc_cbuffer_metainfo(const void* cbuffer, size_t* typesize, int* flags) {
  uint8_t* _src = (uint8_t*)(cbuffer);  /* current pos for source buffer */
  uint8_t version = _src[0];                        /* blosc format version */
  if (version > BLOSC_VERSION_FORMAT) {
    /* Version from future */
    *flags = 0;
    *typesize = 0;
    return;
  }

  /* Read the interesting values */
  *flags = (int)_src[2];                 /* flags */
  *typesize = (size_t)_src[3];           /* typesize */
}