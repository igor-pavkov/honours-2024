void blosc_cbuffer_versions(const void* cbuffer, int* version,
                            int* versionlz) {
  uint8_t* _src = (uint8_t*)(cbuffer);  /* current pos for source buffer */

  /* Read the version info */
  *version = (int)_src[0];         /* blosc format version */
  *versionlz = (int)_src[1];       /* Lempel-Ziv compressor format version */
}