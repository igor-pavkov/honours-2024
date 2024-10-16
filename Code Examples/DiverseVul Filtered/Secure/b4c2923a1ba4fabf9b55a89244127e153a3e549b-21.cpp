static void r_jwe_remove_padding(unsigned char * text, size_t * text_len, unsigned int block_size) {
  unsigned char pad = text[(*text_len)-1], i;
  int pad_ok = 1;

  if (pad && pad < (unsigned char)block_size) {
    for (i=0; i<pad; i++) {
      if (text[((*text_len)-i-1)] != pad) {
        pad_ok = 0;
      }
    }
    if (pad_ok) {
      *text_len -= pad;
    }
  }
}