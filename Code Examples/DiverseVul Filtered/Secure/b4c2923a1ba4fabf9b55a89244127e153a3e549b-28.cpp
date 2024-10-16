static int r_jwe_pbes2_key_unwrap(jwe_t * jwe, jwa_alg alg, jwk_t * jwk, int x5u_flags) {
  unsigned char * salt = NULL, kek[64] = {0}, * key = NULL, cipherkey[128] = {0}, key_data[64] = {0}, * p2s_dec = NULL;
  size_t alg_len, salt_len, key_len = 0, cipherkey_len = 0, p2s_dec_len = 0, kek_len = 0;
  int ret;
  const char * p2s;
  unsigned int p2c;
  gnutls_datum_t password = {NULL, 0}, g_salt = {NULL, 0};
  gnutls_mac_algorithm_t mac = GNUTLS_MAC_UNKNOWN;

  if (r_jwk_key_type(jwk, NULL, x5u_flags) & R_KEY_TYPE_SYMMETRIC) {
    ret = RHN_OK;

    do {
      alg_len = o_strlen(r_jwe_get_header_str_value(jwe, "alg"));
      if ((p2c = (unsigned int)r_jwe_get_header_int_value(jwe, "p2c")) <= 0) {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_pbes2_key_unwrap - Error invalid p2c");
        ret = RHN_ERROR_PARAM;
        break;
      }
      if (o_strlen(r_jwe_get_header_str_value(jwe, "p2s")) < 8) {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_pbes2_key_unwrap - Error invalid p2s");
        ret = RHN_ERROR_PARAM;
        break;
      }
      p2s = r_jwe_get_header_str_value(jwe, "p2s");
      if ((p2s_dec = o_malloc(o_strlen(p2s))) == NULL) {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_pbes2_key_unwrap - Error o_malloc p2s_dec");
        ret = RHN_ERROR_MEMORY;
        break;
      }
      if (!o_base64url_decode((const unsigned char *)p2s, o_strlen(p2s), p2s_dec, &p2s_dec_len)) {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_pbes2_key_unwrap - Error o_base64url_decode p2s_dec");
        ret = RHN_ERROR;
        break;
      }
      salt_len = p2s_dec_len + alg_len + 1;
      if ((salt = o_malloc(salt_len)) == NULL) {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_pbes2_key_unwrap - Error o_malloc salt");
        ret = RHN_ERROR_MEMORY;
        break;
      }
      memcpy(salt, r_jwe_get_header_str_value(jwe, "alg"), alg_len);
      memset(salt+alg_len, 0, 1);
      memcpy(salt+alg_len+1, p2s_dec, p2s_dec_len);

      if (r_jwk_export_to_symmetric_key(jwk, NULL, &key_len) != RHN_OK) {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_pbes2_key_unwrap - Error r_jwk_export_to_symmetric_key (1)");
        ret = RHN_ERROR;
        break;
      }
      key_len += 4;
      if ((key = o_malloc(key_len)) == NULL) {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_pbes2_key_unwrap - Error o_malloc key");
        ret = RHN_ERROR_MEMORY;
        break;
      }
      if (r_jwk_export_to_symmetric_key(jwk, key, &key_len) != RHN_OK) {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_pbes2_key_unwrap - Error r_jwk_export_to_symmetric_key (2)");
        ret = RHN_ERROR;
        break;
      }
      password.data = key;
      password.size = key_len;
      g_salt.data = salt;
      g_salt.size = salt_len;
      if (alg == R_JWA_ALG_PBES2_H256) {
        kek_len = 16;
        mac = GNUTLS_MAC_SHA256;
      } else if (alg == R_JWA_ALG_PBES2_H384) {
        kek_len = 24;
        mac = GNUTLS_MAC_SHA384;
      } else if (alg == R_JWA_ALG_PBES2_H512) {
        kek_len = 32;
        mac = GNUTLS_MAC_SHA512;
      }
      if (gnutls_pbkdf2(mac, &password, &g_salt, p2c, kek, kek_len) != GNUTLS_E_SUCCESS) {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_pbes2_key_unwrap - Error gnutls_pbkdf2");
        ret = RHN_ERROR;
        break;
      }
      if (!o_base64url_decode(jwe->encrypted_key_b64url, o_strlen((const char *)jwe->encrypted_key_b64url), cipherkey, &cipherkey_len)) {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_pbes2_key_unwrap - Error o_base64url_decode cipherkey");
        ret = RHN_ERROR;
        break;
      }
      if (!_r_aes_key_unwrap(kek, kek_len, key_data, cipherkey_len-8, cipherkey)) {
        ret = RHN_ERROR_INVALID;
        break;
      }
      if (r_jwe_set_cypher_key(jwe, key_data, cipherkey_len-8) != RHN_OK) {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_pbes2_key_unwrap - Error r_jwe_set_cypher_key");
        ret = RHN_ERROR;
      }
    } while (0);
    o_free(key);
    o_free(salt);
    o_free(p2s_dec);
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_pbes2_key_unwrap - Error invalid key");
    ret = RHN_ERROR_PARAM;
  }
  return ret;
}