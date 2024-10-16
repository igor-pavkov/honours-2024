static json_t * r_jwe_pbes2_key_wrap(jwe_t * jwe, jwa_alg alg, jwk_t * jwk, int x5u_flags, int * ret) {
  unsigned char salt_seed[_R_PBES_DEFAULT_SALT_LENGTH] = {0}, salt_seed_b64[_R_PBES_DEFAULT_SALT_LENGTH*2], * salt = NULL, kek[64] = {0}, * key = NULL, wrapped_key[72] = {0}, cipherkey_b64url[256] = {0}, * p2s_dec = NULL;
  size_t alg_len, salt_len, key_len = 0, cipherkey_b64url_len = 0, salt_seed_b64_len = 0, p2s_dec_len = 0, kek_len = 0;
  const char * p2s = NULL;
  unsigned int p2c = 0;
  gnutls_datum_t password = {NULL, 0}, g_salt = {NULL, 0};
  gnutls_mac_algorithm_t mac = GNUTLS_MAC_UNKNOWN;
  json_t * j_return = NULL;

  if (r_jwk_key_type(jwk, NULL, x5u_flags) & R_KEY_TYPE_SYMMETRIC) {
    do {
      alg_len = o_strlen(r_jwa_alg_to_str(alg));
      if ((p2s = r_jwe_get_header_str_value(jwe, "p2s")) != NULL) {
        if ((p2s_dec = o_malloc(o_strlen(p2s)*2)) == NULL) {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_pbes2_key_wrap - Error o_malloc p2s_dec");
          *ret = RHN_ERROR_MEMORY;
          break;
        }
        if (!o_base64url_decode((const unsigned char *)p2s, o_strlen(p2s), p2s_dec, &p2s_dec_len)) {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_pbes2_key_wrap - Error o_base64url_decode p2s");
          *ret = RHN_ERROR_PARAM;
          break;
        }
        if (p2s_dec_len < 8) {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_pbes2_key_wrap - Error invalid p2s length");
          *ret = RHN_ERROR_PARAM;
          break;
        }
        salt_len = p2s_dec_len + alg_len + 1;
        if ((salt = o_malloc(salt_len)) == NULL) {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_pbes2_key_wrap - Error o_malloc salt (1)");
          *ret = RHN_ERROR_MEMORY;
          break;
        }
        memcpy(salt, r_jwa_alg_to_str(alg), alg_len);
        memset(salt+alg_len, 0, 1);
        memcpy(salt+alg_len+1, p2s_dec, p2s_dec_len);
      } else {
        if (gnutls_rnd(GNUTLS_RND_NONCE, salt_seed, _R_PBES_DEFAULT_SALT_LENGTH)) {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_pbes2_key_wrap - Error gnutls_rnd");
          *ret = RHN_ERROR;
          break;
        }
        salt_len = _R_PBES_DEFAULT_SALT_LENGTH + alg_len + 1;
        if ((salt = o_malloc(salt_len)) == NULL) {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_pbes2_key_wrap - Error o_malloc salt (2)");
          *ret = RHN_ERROR_MEMORY;
          break;
        }
        if (!o_base64url_encode(salt_seed, _R_PBES_DEFAULT_SALT_LENGTH, salt_seed_b64, &salt_seed_b64_len)) {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_pbes2_key_wrap - Error o_base64url_encode salt_seed");
          *ret = RHN_ERROR;
          break;
        }
        salt_seed_b64[salt_seed_b64_len] = '\0';

        memcpy(salt, r_jwa_alg_to_str(alg), alg_len);
        memset(salt+alg_len, 0, 1);
        memcpy(salt+alg_len+1, salt_seed, _R_PBES_DEFAULT_SALT_LENGTH);
      }
      if ((p2c = (unsigned int)r_jwe_get_header_int_value(jwe, "p2c")) <= 0) {
        p2c = _R_PBES_DEFAULT_ITERATION;
      }

      if (r_jwk_export_to_symmetric_key(jwk, NULL, &key_len) != RHN_OK) {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_pbes2_key_wrap - Error r_jwk_export_to_symmetric_key (1)");
        *ret = RHN_ERROR;
        break;
      }
      key_len += 4;
      if ((key = o_malloc(key_len)) == NULL) {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_pbes2_key_wrap - Error o_malloc key");
        *ret = RHN_ERROR_MEMORY;
        break;
      }
      if (r_jwk_export_to_symmetric_key(jwk, key, &key_len) != RHN_OK) {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_pbes2_key_wrap - Error r_jwk_export_to_symmetric_key (2)");
        *ret = RHN_ERROR;
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
        *ret = RHN_ERROR;
        break;
      }
      _r_aes_key_wrap(kek, kek_len, jwe->key, jwe->key_len, wrapped_key);
      if (!o_base64url_encode(wrapped_key, jwe->key_len+8, cipherkey_b64url, &cipherkey_b64url_len)) {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_aes_key_wrap - Error o_base64url_encode wrapped_key");
        *ret = RHN_ERROR;
        break;
      }
      j_return = json_pack("{ss%s{sssssi}}", "encrypted_key", cipherkey_b64url, cipherkey_b64url_len,
                                             "header",
                                               "alg", r_jwa_alg_to_str(alg),
                                               "p2s", p2s!=NULL?p2s:(const char*)salt_seed_b64,
                                               "p2c", p2c);
    } while (0);
    o_free(key);
    o_free(salt);
    o_free(p2s_dec);
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_pbes2_key_wrap - Error invalid key");
  }
  return j_return;
}