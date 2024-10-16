static json_t * r_jwe_perform_key_encryption(jwe_t * jwe, jwa_alg alg, jwk_t * jwk, int x5u_flags, int * ret) {
  json_t * j_return = NULL;
  int res;
  unsigned int bits = 0;
  gnutls_pubkey_t g_pub = NULL;
  gnutls_datum_t plainkey, cypherkey = {NULL, 0};
  unsigned char * cypherkey_b64 = NULL, key[128] = {0};
  size_t cypherkey_b64_len = 0, key_len = 0, index = 0;
  const char * key_ref = NULL;
  json_t * j_element = NULL, * j_reference, * j_key_ref_array;
#if NETTLE_VERSION_NUMBER >= 0x030400
  uint8_t * cyphertext = NULL;
  size_t cyphertext_len = 0;
#endif
#if NETTLE_VERSION_NUMBER >= 0x030600
  json_t * jwk_priv = NULL;
#endif

  switch (alg) {
    case R_JWA_ALG_RSA1_5:
      res = r_jwk_key_type(jwk, &bits, x5u_flags);
      if (res & (R_KEY_TYPE_RSA|R_KEY_TYPE_PUBLIC) && bits >= 2048) {
        if (jwk != NULL && (g_pub = r_jwk_export_to_gnutls_pubkey(jwk, x5u_flags)) != NULL) {
          plainkey.data = jwe->key;
          plainkey.size = jwe->key_len;
          if (!(res = gnutls_pubkey_encrypt_data(g_pub, 0, &plainkey, &cypherkey))) {
            if ((cypherkey_b64 = o_malloc(cypherkey.size*2)) != NULL) {
              if (o_base64url_encode(cypherkey.data, cypherkey.size, cypherkey_b64, &cypherkey_b64_len)) {
                j_return = json_pack("{ss%s{ss}}", "encrypted_key", cypherkey_b64, cypherkey_b64_len, "header", "alg", r_jwa_alg_to_str(alg));
              } else {
                y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_perform_key_encryption - Error o_base64url_encode cypherkey_b64");
                *ret = RHN_ERROR;
              }
              o_free(cypherkey_b64);
            } else {
              y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_perform_key_encryption - Error o_malloc cypherkey_b64");
              *ret = RHN_ERROR_MEMORY;
            }
            gnutls_free(cypherkey.data);
          } else {
            y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_perform_key_encryption - Error gnutls_pubkey_encrypt_data: %s", gnutls_strerror(res));
            *ret = RHN_ERROR;
          }
        } else {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_perform_key_encryption - Unable to export public key");
          *ret = RHN_ERROR;
        }
        gnutls_pubkey_deinit(g_pub);
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_perform_key_encryption - Error invalid key type");
        *ret = RHN_ERROR_PARAM;
      }
      break;
#if NETTLE_VERSION_NUMBER >= 0x030400
    case R_JWA_ALG_RSA_OAEP:
    case R_JWA_ALG_RSA_OAEP_256:
      res = r_jwk_key_type(jwk, &bits, x5u_flags);
      if (res & (R_KEY_TYPE_RSA|R_KEY_TYPE_PUBLIC) && bits >= 2048) {
        if (jwk != NULL && (g_pub = r_jwk_export_to_gnutls_pubkey(jwk, x5u_flags)) != NULL) {
          if ((cyphertext = o_malloc(bits+1)) != NULL) {
            cyphertext_len = bits+1;
            if (_r_rsa_oaep_encrypt(g_pub, alg, jwe->key, jwe->key_len, cyphertext, &cyphertext_len) == RHN_OK) {
              if ((cypherkey_b64 = o_malloc(cyphertext_len*2)) != NULL) {
                if (o_base64url_encode(cyphertext, cyphertext_len, cypherkey_b64, &cypherkey_b64_len)) {
                  j_return = json_pack("{ss%s{ss}}", "encrypted_key", cypherkey_b64, cypherkey_b64_len, "header", "alg", r_jwa_alg_to_str(alg));
                } else {
                  y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_perform_key_encryption - Error o_base64url_encode cypherkey_b64");
                  *ret = RHN_ERROR;
                }
                o_free(cypherkey_b64);
              } else {
                y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_perform_key_encryption - Error o_malloc cypherkey_b64");
                *ret = RHN_ERROR_MEMORY;
              }
            } else {
              y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_perform_key_encryption - Error _r_rsa_oaep_encrypt");
              *ret = RHN_ERROR;
            }
          } else {
            y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_perform_key_encryption - Error allocating resources for cyphertext");
            *ret = RHN_ERROR_MEMORY;
          }
          o_free(cyphertext);
        } else {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_perform_key_encryption - Unable to export public key");
          *ret = RHN_ERROR;
        }
        gnutls_pubkey_deinit(g_pub);
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_perform_key_encryption - Error invalid key type");
        *ret = RHN_ERROR_PARAM;
      }
      break;
#endif
    case R_JWA_ALG_DIR:
      o_free(jwe->encrypted_key_b64url);
      jwe->encrypted_key_b64url = NULL;
      if (jwk != NULL) {
        if (r_jwk_key_type(jwk, &bits, x5u_flags) & R_KEY_TYPE_SYMMETRIC && bits == _r_get_key_size(jwe->enc)*8) {
          key_len = bits/8;
          if (r_jwk_export_to_symmetric_key(jwk, key, &key_len) != RHN_OK) {
            y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_perform_key_encryption - Error r_jwk_export_to_symmetric_key");
            *ret = RHN_ERROR;
          } else {
            if (r_jwe_set_cypher_key(jwe, key, key_len) != RHN_OK) {
              y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_perform_key_encryption - Error r_jwe_set_cypher_key");
              *ret = RHN_ERROR;
            } else {
              j_return = json_object();
            }
          }
        } else {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_perform_key_encryption - Error invalid key type");
          *ret = RHN_ERROR_PARAM;
        }
      } else if (jwe->key != NULL && jwe->key_len > 0) {
        j_return = json_object();
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_perform_key_encryption - Error no key available for alg 'dir'");
        *ret = RHN_ERROR_PARAM;
      }
      break;
    case R_JWA_ALG_A128GCMKW:
#if GNUTLS_VERSION_NUMBER >= 0x03060e
    case R_JWA_ALG_A192GCMKW:
#endif
    case R_JWA_ALG_A256GCMKW:
      if ((j_return = r_jwe_aesgcm_key_wrap(jwe, alg, jwk, x5u_flags, ret)) == NULL) {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_perform_key_encryption - Error r_jwe_aesgcm_key_wrap");
      }
      break;
#if NETTLE_VERSION_NUMBER >= 0x030400
    case R_JWA_ALG_A128KW:
    case R_JWA_ALG_A192KW:
    case R_JWA_ALG_A256KW:
      if ((j_return = r_jwe_aes_key_wrap(jwe, alg, jwk, x5u_flags, ret)) == NULL) {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_perform_key_encryption - Error r_jwe_aes_key_wrap");
      }
      break;
#endif
#if GNUTLS_VERSION_NUMBER >= 0x03060d
    case R_JWA_ALG_PBES2_H256:
    case R_JWA_ALG_PBES2_H384:
    case R_JWA_ALG_PBES2_H512:
      if ((j_return = r_jwe_pbes2_key_wrap(jwe, alg, jwk, x5u_flags, ret)) == NULL) {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_perform_key_encryption - Error r_jwe_pbes2_key_wrap");
      }
      break;
#endif
#if NETTLE_VERSION_NUMBER >= 0x030600
    case R_JWA_ALG_ECDH_ES:
    case R_JWA_ALG_ECDH_ES_A128KW:
    case R_JWA_ALG_ECDH_ES_A192KW:
    case R_JWA_ALG_ECDH_ES_A256KW:
      res = r_jwk_key_type(jwk, &bits, x5u_flags);
      if (res & (R_KEY_TYPE_EC|R_KEY_TYPE_PUBLIC)) {
        if (r_jwks_size(jwe->jwks_privkey) == 1) {
          jwk_priv = r_jwks_get_at(jwe->jwks_privkey, 0);
        }
        if ((j_return = _r_jwe_ecdh_encrypt(jwe, alg, jwk, jwk_priv, res, bits, x5u_flags, ret)) == NULL) {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_perform_key_encryption - Error _r_jwe_ecdh_encrypt");
        }
        r_jwk_free(jwk_priv);
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_perform_key_encryption - invalid public key type");
        *ret = RHN_ERROR_PARAM;
      }
      break;
#endif
    default:
      y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_perform_key_encryption - Unsupported alg");
      *ret = RHN_ERROR_PARAM;
      break;
  }
  j_key_ref_array = json_array();
  json_object_foreach(json_object_get(j_return, "header"), key_ref, j_element) {
    j_reference = json_object_get(jwe->j_header, key_ref);
    if (j_reference == NULL) {
      j_reference = json_object_get(jwe->j_unprotected_header, key_ref);
    }
    if (j_reference != NULL && json_equal(j_reference, j_element)) {
      json_array_append_new(j_key_ref_array, json_string(key_ref));
    }
  }
  json_array_foreach(j_key_ref_array, index, j_element) {
    json_object_del(json_object_get(j_return, "header"), json_string_value(j_element));
  }
  json_decref(j_key_ref_array);
  if (!json_object_size(json_object_get(j_return, "header"))) {
    json_object_del(j_return, "header");
  }
  return j_return;
}