const char* blosc_cbuffer_complib(const void* cbuffer) {
  uint8_t* _src = (uint8_t*)(cbuffer);  /* current pos for source buffer */
  int clibcode;
  const char* complib;

  /* Read the compressor format/library info */
  clibcode = (_src[2] & 0xe0) >> 5;
  complib = clibcode_to_clibname(clibcode);
  return complib;
}