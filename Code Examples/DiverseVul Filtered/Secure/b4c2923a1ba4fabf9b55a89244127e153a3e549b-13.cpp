static int r_jwe_aes_key_unwrap(jwe_t * jwe, jwa_alg alg, jwk_t * jwk, int x5u_flags) {
  int ret;
  uint8_t kek[32] = {0}, key_data[64], cipherkey[128] = {0};
  size_t kek_len = 0, cipherkey_len = 0;
  unsigned int bits = 0;

  if (r_jwk_key_type(jwk, &bits, x5u_flags) & R_KEY_TYPE_SYMMETRIC) {
    ret = RHN_OK;

    do {
      if (alg == R_JWA_ALG_A128KW && bits != 128) {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_aes_key_unwrap - Error invalid key size, expected 128 bits");
        ret = RHN_ERROR_INVALID;
        break;
      }
      if (alg == R_JWA_ALG_A192KW && bits != 192) {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_aes_key_unwrap - Error invalid key size, expected 192 bits");
        ret = RHN_ERROR_INVALID;
        break;
      }
      if (alg == R_JWA_ALG_A256KW && bits != 256) {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_aes_key_unwrap - Error invalid key size, expected 256 bits");
        ret = RHN_ERROR_INVALID;
        break;
      }
      if (r_jwk_export_to_symmetric_key(jwk, kek, &kek_len) != RHN_OK) {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_aes_key_unwrap - Error r_jwk_export_to_symmetric_key");
        ret = RHN_ERROR;
        break;
      }
      if (!o_base64url_decode(jwe->encrypted_key_b64url, o_strlen((const char *)jwe->encrypted_key_b64url), NULL, &cipherkey_len)) {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_aes_key_unwrap - Error o_base64url_decode cipherkey");
        ret = RHN_ERROR_INVALID;
        break;
      }
      if (cipherkey_len > 72) {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_aes_key_unwrap - Error invalid cipherkey len");
        ret = RHN_ERROR_INVALID;
        break;
      }
      if (!o_base64url_decode(jwe->encrypted_key_b64url, o_strlen((const char *)jwe->encrypted_key_b64url), cipherkey, &cipherkey_len)) {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_aes_key_unwrap - Error o_base64url_decode cipherkey");
        ret = RHN_ERROR_INVALID;
        break;
      }
      if (!_r_aes_key_unwrap(kek, kek_len, key_data, cipherkey_len-8, cipherkey)) {
        ret = RHN_ERROR_INVALID;
        break;
      }
      if (r_jwe_set_cypher_key(jwe, key_data, cipherkey_len-8) != RHN_OK) {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_aes_key_unwrap - Error r_jwe_set_cypher_key");
        ret = RHN_ERROR;
      }
    } while (0);
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_aes_key_unwrap - Error invalid key");
    ret = RHN_ERROR_INVALID;
  }
  return ret;
}