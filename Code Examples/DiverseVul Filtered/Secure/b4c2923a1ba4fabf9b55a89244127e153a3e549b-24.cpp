static json_t * r_jwe_aesgcm_key_wrap(jwe_t * jwe, jwa_alg alg, jwk_t * jwk, int x5u_flags, int * ret) {
  int res;
  unsigned char iv[96] = {0}, iv_b64url[192] = {0}, * key = NULL, cipherkey[64] = {0}, cipherkey_b64url[128] = {0}, tag[128] = {0}, tag_b64url[256] = {0};
  size_t iv_b64url_len = 0, key_len = 0, cipherkey_b64url_len = 0, tag_b64url_len = 0, iv_size = gnutls_cipher_get_iv_size(r_jwe_get_alg_from_alg(alg)), tag_len = gnutls_cipher_get_tag_size(r_jwe_get_alg_from_alg(alg));
  unsigned int bits = 0;
  gnutls_datum_t key_g, iv_g;
  gnutls_cipher_hd_t handle = NULL;
  json_t * j_return = NULL;

  if (r_jwk_key_type(jwk, &bits, x5u_flags) & R_KEY_TYPE_SYMMETRIC) {
    key_len = bits;

    do {
      if ((key = o_malloc(key_len+4)) == NULL) {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_aesgcm_key_wrap - Error allocating resources for key");
        *ret = RHN_ERROR_MEMORY;
        break;
      }
      if (r_jwk_export_to_symmetric_key(jwk, key, &key_len) != RHN_OK) {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_aesgcm_key_wrap - Error r_jwk_export_to_symmetric_key");
        *ret = RHN_ERROR_PARAM;
        break;
      }
      if (r_jwe_get_header_str_value(jwe, "iv") == NULL) {
        if (gnutls_rnd(GNUTLS_RND_NONCE, iv, iv_size)) {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_aesgcm_key_wrap - Error gnutls_rnd");
          *ret = RHN_ERROR;
          break;
        }
        if (!o_base64url_encode(iv, iv_size, iv_b64url, &iv_b64url_len)) {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_aesgcm_key_wrap - Error o_base64url_encode iv");
          *ret = RHN_ERROR;
          break;
        }
        iv_b64url[iv_b64url_len] = '\0';
      } else {
        if (!o_base64url_decode((const unsigned char *)r_jwe_get_header_str_value(jwe, "iv"), o_strlen(r_jwe_get_header_str_value(jwe, "iv")), iv, &iv_size)) {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_aesgcm_key_wrap - Error o_base64url_decode iv");
          *ret = RHN_ERROR_PARAM;
          break;
        }
        if (iv_size != gnutls_cipher_get_iv_size(r_jwe_get_alg_from_alg(alg))) {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_aesgcm_key_wrap - Error invalid iv size");
          *ret = RHN_ERROR_PARAM;
          break;
        }
      }
      key_g.data = key;
      key_g.size = key_len;
      iv_g.data = iv;
      iv_g.size = iv_size;
      if ((res = gnutls_cipher_init(&handle, r_jwe_get_alg_from_alg(alg), &key_g, &iv_g))) {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_aesgcm_key_wrap - Error gnutls_cipher_init: '%s'", gnutls_strerror(res));
        *ret = RHN_ERROR_PARAM;
        break;
      }
      if ((res = gnutls_cipher_encrypt2(handle, jwe->key, jwe->key_len, cipherkey, jwe->key_len))) {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_aesgcm_key_wrap - Error gnutls_cipher_encrypt2: '%s'", gnutls_strerror(res));
        *ret = RHN_ERROR;
        break;
      }
      if (!o_base64url_encode(cipherkey, jwe->key_len, cipherkey_b64url, &cipherkey_b64url_len)) {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_aesgcm_key_wrap - Error o_base64url_encode cipherkey");
        *ret = RHN_ERROR;
        break;
      }
      if ((res = gnutls_cipher_tag(handle, tag, tag_len))) {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_aesgcm_key_wrap - Error gnutls_cipher_tag: '%s'", gnutls_strerror(res));
        *ret = RHN_ERROR;
        break;
      }
      if (!o_base64url_encode(tag, tag_len, tag_b64url, &tag_b64url_len)) {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_aesgcm_key_wrap - Error o_base64url_encode tag");
        *ret = RHN_ERROR;
        break;
      }
      tag_b64url[tag_b64url_len] = '\0';
      j_return = json_pack("{ss%s{ssssss}}", "encrypted_key", cipherkey_b64url, cipherkey_b64url_len,
                                             "header",
                                               "iv", r_jwe_get_header_str_value(jwe, "iv")==NULL?(const char *)iv_b64url:r_jwe_get_header_str_value(jwe, "iv"),
                                               "tag", tag_b64url,
                                               "alg", r_jwa_alg_to_str(alg));
    } while (0);
    o_free(key);
    if (handle != NULL) {
      gnutls_cipher_deinit(handle);
    }
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_aesgcm_key_wrap - Error invalid key");
    *ret = RHN_ERROR_PARAM;
  }
  return j_return;
}