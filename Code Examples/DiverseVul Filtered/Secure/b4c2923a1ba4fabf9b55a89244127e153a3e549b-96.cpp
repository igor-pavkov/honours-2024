static int r_preform_key_decryption(jwe_t * jwe, jwa_alg alg, jwk_t * jwk, int x5u_flags) {
  int ret, res;
  gnutls_datum_t plainkey = {NULL, 0}, cypherkey;
  gnutls_privkey_t g_priv = NULL;
  unsigned int bits = 0;
  unsigned char * cypherkey_dec = NULL, * key = NULL;
  size_t cypherkey_dec_len = 0, key_len = 0;
#if NETTLE_VERSION_NUMBER >= 0x030400
  uint8_t * clearkey = NULL;
  size_t clearkey_len = 0;
#endif

  switch (alg) {
    case R_JWA_ALG_RSA1_5:
      if (r_jwk_key_type(jwk, &bits, x5u_flags) & (R_KEY_TYPE_RSA|R_KEY_TYPE_PRIVATE) && bits >= 2048) {
        if (jwk != NULL && !o_strnullempty((const char *)jwe->encrypted_key_b64url) && (g_priv = r_jwk_export_to_gnutls_privkey(jwk)) != NULL) {
          if ((cypherkey_dec = o_malloc(o_strlen((const char *)jwe->encrypted_key_b64url))) != NULL) {
            memset(cypherkey_dec, 0, o_strlen((const char *)jwe->encrypted_key_b64url));
            if (o_base64url_decode(jwe->encrypted_key_b64url, o_strlen((const char *)jwe->encrypted_key_b64url), cypherkey_dec, &cypherkey_dec_len)) {
              cypherkey.size = cypherkey_dec_len;
              cypherkey.data = cypherkey_dec;
              if (!(res = gnutls_privkey_decrypt_data(g_priv, 0, &cypherkey, &plainkey))) {
                if (r_jwe_set_cypher_key(jwe, plainkey.data, plainkey.size) == RHN_OK) {
                  ret = RHN_OK;
                } else {
                  y_log_message(Y_LOG_LEVEL_ERROR, "r_preform_key_decryption - Error r_jwe_set_cypher_key (RSA1_5)");
                  ret = RHN_ERROR;
                }
                gnutls_free(plainkey.data);
              } else if (res == GNUTLS_E_DECRYPTION_FAILED) {
                ret = RHN_ERROR_INVALID;
              } else {
                y_log_message(Y_LOG_LEVEL_ERROR, "r_preform_key_decryption - Error gnutls_privkey_decrypt_data: %s", gnutls_strerror(res));
                ret = RHN_ERROR;
              }
            } else {
              y_log_message(Y_LOG_LEVEL_ERROR, "r_preform_key_decryption - Error o_base64url_decode cypherkey_dec");
              ret = RHN_ERROR_PARAM;
            }
            o_free(cypherkey_dec);
          } else {
            y_log_message(Y_LOG_LEVEL_ERROR, "r_preform_key_decryption - Error o_malloc cypherkey_dec");
            ret = RHN_ERROR_MEMORY;
          }
        } else {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_preform_key_decryption - Error invalid RSA1_5 input parameters");
          ret = RHN_ERROR_PARAM;
        }
        gnutls_privkey_deinit(g_priv);
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_preform_key_decryption - Error invalid key size RSA1_5");
        ret = RHN_ERROR_INVALID;
      }
      break;
#if NETTLE_VERSION_NUMBER >= 0x030400
    case R_JWA_ALG_RSA_OAEP:
    case R_JWA_ALG_RSA_OAEP_256:
      if (r_jwk_key_type(jwk, &bits, x5u_flags) & (R_KEY_TYPE_RSA|R_KEY_TYPE_PRIVATE) && bits >= 2048) {
        if (jwk != NULL && !o_strnullempty((const char *)jwe->encrypted_key_b64url) && (g_priv = r_jwk_export_to_gnutls_privkey(jwk)) != NULL) {
          if ((cypherkey_dec = o_malloc(o_strlen((const char *)jwe->encrypted_key_b64url))) != NULL) {
            if (o_base64url_decode(jwe->encrypted_key_b64url, o_strlen((const char *)jwe->encrypted_key_b64url), cypherkey_dec, &cypherkey_dec_len)) {
              if ((clearkey = o_malloc(bits+1)) != NULL) {
                clearkey_len = bits+1;
                if (_r_rsa_oaep_decrypt(g_priv, alg, cypherkey_dec, cypherkey_dec_len, clearkey, &clearkey_len) == RHN_OK) {
                  if (_r_get_key_size(jwe->enc) == clearkey_len) {
                    if (r_jwe_set_cypher_key(jwe, clearkey, clearkey_len) == RHN_OK) {
                      ret = RHN_OK;
                    } else {
                      y_log_message(Y_LOG_LEVEL_ERROR, "r_preform_key_decryption - Error r_jwe_set_cypher_key (RSA_OAEP)");
                      ret = RHN_ERROR;
                    }
                  } else {
                    y_log_message(Y_LOG_LEVEL_ERROR, "r_preform_key_decryption - Error invalid key length");
                    ret = RHN_ERROR_PARAM;
                  }
                } else {
                  y_log_message(Y_LOG_LEVEL_ERROR, "r_preform_key_decryption - Error _r_rsa_oaep_decrypt");
                  ret = RHN_ERROR_INVALID;
                }
              } else {
                y_log_message(Y_LOG_LEVEL_ERROR, "r_preform_key_decryption - Error o_malloc clearkey");
                ret = RHN_ERROR_MEMORY;
              }
              o_free(clearkey);
            } else {
              y_log_message(Y_LOG_LEVEL_ERROR, "r_preform_key_decryption - Error o_base64url_decode cypherkey_dec");
              ret = RHN_ERROR_PARAM;
            }
            o_free(cypherkey_dec);
          } else {
            y_log_message(Y_LOG_LEVEL_ERROR, "r_preform_key_decryption - Error o_malloc cypherkey_dec");
            ret = RHN_ERROR_MEMORY;
          }
        } else {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_preform_key_decryption - Error invalid RSA1-OAEP input parameters");
          ret = RHN_ERROR_PARAM;
        }
        gnutls_privkey_deinit(g_priv);
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_preform_key_decryption - Error invalid key size RSA_OAEP");
        ret = RHN_ERROR_INVALID;
      }
      break;
#endif
    case R_JWA_ALG_DIR:
      o_free(jwe->encrypted_key_b64url);
      jwe->encrypted_key_b64url = NULL;
      if (jwk != NULL) {
        if (r_jwk_key_type(jwk, &bits, x5u_flags) & R_KEY_TYPE_SYMMETRIC && bits == _r_get_key_size(jwe->enc)*8) {
          key_len = (size_t)(bits/8);
          if ((key = o_malloc(key_len+4)) != NULL) {
            if (r_jwk_export_to_symmetric_key(jwk, key, &key_len) == RHN_OK) {
              o_free(jwe->encrypted_key_b64url);
              jwe->encrypted_key_b64url = NULL;
              ret = r_jwe_set_cypher_key(jwe, key, key_len);
            } else {
              y_log_message(Y_LOG_LEVEL_ERROR, "r_preform_key_decryption - Error r_jwk_export_to_symmetric_key");
              ret = RHN_ERROR_MEMORY;
            }
            o_free(key);
          } else {
            y_log_message(Y_LOG_LEVEL_ERROR, "r_preform_key_decryption - Error allocating resources for key");
            ret = RHN_ERROR_MEMORY;
          }
        } else {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_preform_key_decryption - Error invalid key type");
          ret = RHN_ERROR_PARAM;
        }
      } else if (jwe->key != NULL && jwe->key_len > 0) {
        ret = RHN_OK;
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_preform_key_decryption - Error no key available for alg 'dir'");
        ret = RHN_ERROR_INVALID;
      }
      break;
    case R_JWA_ALG_A128GCMKW:
#if GNUTLS_VERSION_NUMBER >= 0x03060e
    case R_JWA_ALG_A192GCMKW:
#endif
    case R_JWA_ALG_A256GCMKW:
      if ((res = r_jwe_aesgcm_key_unwrap(jwe, alg, jwk, x5u_flags)) == RHN_OK) {
        ret = RHN_OK;
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_preform_key_decryption - Error r_jwe_aesgcm_key_unwrap");
        ret = res;
      }
      break;
#if NETTLE_VERSION_NUMBER >= 0x030400
    case R_JWA_ALG_A128KW:
    case R_JWA_ALG_A192KW:
    case R_JWA_ALG_A256KW:
      if ((res = r_jwe_aes_key_unwrap(jwe, alg, jwk, x5u_flags)) == RHN_OK) {
        ret = RHN_OK;
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_preform_key_decryption - Error r_jwe_aes_key_unwrap");
        ret = res;
      }
      break;
#endif
#if GNUTLS_VERSION_NUMBER >= 0x03060d
    case R_JWA_ALG_PBES2_H256:
    case R_JWA_ALG_PBES2_H384:
    case R_JWA_ALG_PBES2_H512:
      if ((res = r_jwe_pbes2_key_unwrap(jwe, alg, jwk, x5u_flags)) == RHN_OK) {
        ret = RHN_OK;
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_preform_key_decryption - Error r_jwe_pbes2_key_unwrap");
        ret = res;
      }
      break;
#endif
#if NETTLE_VERSION_NUMBER >= 0x030600
    case R_JWA_ALG_ECDH_ES:
    case R_JWA_ALG_ECDH_ES_A128KW:
    case R_JWA_ALG_ECDH_ES_A192KW:
    case R_JWA_ALG_ECDH_ES_A256KW:
      res = r_jwk_key_type(jwk, &bits, x5u_flags);
      if (res & (R_KEY_TYPE_EC|R_KEY_TYPE_PRIVATE) || res & (R_KEY_TYPE_EDDSA|R_KEY_TYPE_PRIVATE)) {
        if ((res = _r_jwe_ecdh_decrypt(jwe, alg, jwk, res, bits, x5u_flags)) == RHN_OK) {
          ret = RHN_OK;
        } else {
          if (res != RHN_ERROR_INVALID) {
            y_log_message(Y_LOG_LEVEL_ERROR, "r_preform_key_decryption - Error _r_jwe_ecdh_decrypt");
          }
          ret = res;
        }
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_preform_key_decryption - invalid key type %d", res);
        ret = RHN_ERROR_INVALID;
      }
      break;
#endif
    default:
      y_log_message(Y_LOG_LEVEL_ERROR, "r_preform_key_decryption - Error unsupported algorithm");
      ret = RHN_ERROR_INVALID;
      break;
  }
  return ret;
}