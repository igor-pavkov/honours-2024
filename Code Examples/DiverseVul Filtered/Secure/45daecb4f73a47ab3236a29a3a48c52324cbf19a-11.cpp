static unsigned int read_partition_size(VP8D_COMP *pbi,
                                        const unsigned char *cx_size) {
  unsigned char temp[3];
  if (pbi->decrypt_cb) {
    pbi->decrypt_cb(pbi->decrypt_state, cx_size, temp, 3);
    cx_size = temp;
  }
  return cx_size[0] + (cx_size[1] << 8) + (cx_size[2] << 16);
}