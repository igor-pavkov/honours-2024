static json_t * r_jwe_aes_key_wrap(jwe_t * jwe, jwa_alg alg, jwk_t * jwk, int x5u_flags, int * ret) {
  uint8_t kek[32] = {0}, wrapped_key[72] = {0};
  unsigned char cipherkey_b64url[256] = {0};
  size_t kek_len = 0, cipherkey_b64url_len = 0;
  unsigned int bits = 0;
  json_t * j_return = NULL;

  if (r_jwk_key_type(jwk, &bits, x5u_flags) & R_KEY_TYPE_SYMMETRIC) {
    do {
      if (alg == R_JWA_ALG_A128KW && bits != 128) {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_aes_key_wrap - Error invalid key size, expected 128 bits");
        *ret = RHN_ERROR_PARAM;
        break;
      }
      if (alg == R_JWA_ALG_A192KW && bits != 192) {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_aes_key_wrap - Error invalid key size, expected 192 bits");
        *ret = RHN_ERROR_PARAM;
        break;
      }
      if (alg == R_JWA_ALG_A256KW && bits != 256) {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_aes_key_wrap - Error invalid key size, expected 256 bits");
        *ret = RHN_ERROR_PARAM;
        break;
      }
      if (r_jwk_export_to_symmetric_key(jwk, kek, &kek_len) != RHN_OK) {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_aes_key_wrap - Error r_jwk_export_to_symmetric_key");
        *ret = RHN_ERROR;
        break;
      }
      _r_aes_key_wrap(kek, kek_len, jwe->key, jwe->key_len, wrapped_key);
      if (!o_base64url_encode(wrapped_key, jwe->key_len+8, cipherkey_b64url, &cipherkey_b64url_len)) {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_aes_key_wrap - Error o_base64url_encode wrapped_key");
        *ret = RHN_ERROR;
        break;
      }
      j_return = json_pack("{ss%s{ss}}", "encrypted_key", cipherkey_b64url, cipherkey_b64url_len, "header", "alg", r_jwa_alg_to_str(alg));
      o_free(jwe->encrypted_key_b64url);
      jwe->encrypted_key_b64url = (unsigned char *)o_strndup((const char *)cipherkey_b64url, cipherkey_b64url_len);
    } while (0);
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_aes_key_wrap - Error invalid key");
    *ret = RHN_ERROR_PARAM;
  }
  return j_return;
}