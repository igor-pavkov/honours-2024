static int r_jwe_compute_hmac_tag(jwe_t * jwe, unsigned char * ciphertext, size_t cyphertext_len, const unsigned char * aad, unsigned char * tag, size_t * tag_len) {
  int ret, res;
  unsigned char al[8], * compute_hmac = NULL;
  uint64_t aad_len;
  size_t hmac_size = 0, aad_size = o_strlen((const char *)aad), i;
  gnutls_mac_algorithm_t mac = r_jwe_get_digest_from_enc(jwe->enc);

  aad_len = (uint64_t)(o_strlen((const char *)aad)*8);
  memset(al, 0, 8);
  for(i = 0; i < 8; i++) {
    al[i] = (uint8_t)((aad_len >> 8*(7 - i)) & 0xFF);
  }

  if ((compute_hmac = o_malloc(aad_size+jwe->iv_len+cyphertext_len+8)) != NULL) {
    if (aad_size) {
      memcpy(compute_hmac, aad, aad_size);
      hmac_size += aad_size;
    }
    memcpy(compute_hmac+hmac_size, jwe->iv, jwe->iv_len);
    hmac_size += jwe->iv_len;
    memcpy(compute_hmac+hmac_size, ciphertext, cyphertext_len);
    hmac_size += cyphertext_len;
    memcpy(compute_hmac+hmac_size, al, 8);
    hmac_size += 8;

    if (!(res = gnutls_hmac_fast(mac, jwe->key, jwe->key_len/2, compute_hmac, hmac_size, tag))) {
      *tag_len = gnutls_hmac_get_len(mac)/2;
      ret = RHN_OK;
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_compute_hmac_tag - Error gnutls_hmac_fast: '%s'", gnutls_strerror(res));
      ret = RHN_ERROR;
    }
    o_free(compute_hmac);
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_compute_hmac_tag - Error allocating resources for compute_hmac");
    ret = RHN_ERROR;
  }
  return ret;
}