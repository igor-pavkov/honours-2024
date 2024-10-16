static int r_jwe_set_ptext_with_block(unsigned char * data, size_t data_len, unsigned char ** ptext, size_t * ptext_len, gnutls_cipher_algorithm_t alg, int cipher_cbc) {
  size_t b_size = (size_t)gnutls_cipher_get_block_size(alg);
  int ret;

  *ptext = NULL;
  if (cipher_cbc) {
    if (data_len % b_size) {
      *ptext_len = ((data_len/b_size)+1)*b_size;
    } else {
      *ptext_len = data_len;
    }
    if (*ptext_len) {
      if ((*ptext = o_malloc(*ptext_len)) != NULL) {
        memcpy(*ptext, data, data_len);
        memset(*ptext+data_len, (*ptext_len)-data_len, (*ptext_len)-data_len);
        ret = RHN_OK;
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_set_ptext_with_block - Error allocating resources for ptext (1)");
        ret = RHN_ERROR_MEMORY;
      }
    } else {
      ret = RHN_ERROR;
    }
  } else {
    *ptext_len = data_len;
    if ((*ptext = o_malloc(data_len)) != NULL) {
      memcpy(*ptext, data, data_len);
      ret = RHN_OK;
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_set_ptext_with_block - Error allocating resources for ptext (2)");
      ret = RHN_ERROR_MEMORY;
    }
  }
  return ret;
}