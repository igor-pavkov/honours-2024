int r_jwe_decrypt_payload(jwe_t * jwe) {
  int ret = RHN_OK, res;
  gnutls_cipher_hd_t handle;
  gnutls_datum_t key, iv;
  unsigned char * payload_enc = NULL, * ciphertext = NULL, * unzip = NULL, * aad = NULL;
  size_t payload_enc_len = 0, ciphertext_len = 0, unzip_len = 0;
  unsigned char tag[128], * tag_b64url = NULL;
  size_t tag_len = 0, tag_b64url_len = 0;
  int cipher_cbc;

  if (jwe != NULL && jwe->enc != R_JWA_ENC_UNKNOWN && !o_strnullempty((const char *)jwe->ciphertext_b64url) && !o_strnullempty((const char *)jwe->iv_b64url) && jwe->key != NULL && jwe->key_len && jwe->key_len == _r_get_key_size(jwe->enc)) {
    // Decode iv and payload_b64
    o_free(jwe->iv);
    if ((jwe->iv = o_malloc(o_strlen((const char *)jwe->iv_b64url))) != NULL) {
      if (o_base64url_decode(jwe->iv_b64url, o_strlen((const char *)jwe->iv_b64url), jwe->iv, &jwe->iv_len)) {
        if ((jwe->iv = o_realloc(jwe->iv, jwe->iv_len)) != NULL) {
          if ((payload_enc = o_malloc(o_strlen((const char *)jwe->ciphertext_b64url))) != NULL && (ciphertext = o_malloc(o_strlen((const char *)jwe->ciphertext_b64url))) != NULL) {
            if (!o_base64url_decode(jwe->ciphertext_b64url, o_strlen((const char *)jwe->ciphertext_b64url), ciphertext, &ciphertext_len)) {
              y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_decrypt_payload - Error o_base64url_decode ciphertext");
              ret = RHN_ERROR;
            }
          } else {
            y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_decrypt_payload - Error allocating resources for payload_enc or ciphertext");
            ret = RHN_ERROR_MEMORY;
          }
        } else {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_decrypt_payload - Error reallocating resources for iv");
          ret = RHN_ERROR_MEMORY;
        }
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_decrypt_payload - Error o_base64url_decode iv");
        ret = RHN_ERROR;
      }
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_decrypt_payload - Error allocating resources for iv");
      ret = RHN_ERROR_MEMORY;
    }

    if (ret == RHN_OK) {
      if (jwe->enc == R_JWA_ENC_A128CBC || jwe->enc == R_JWA_ENC_A192CBC || jwe->enc == R_JWA_ENC_A256CBC) {
        key.data = jwe->key+(jwe->key_len/2);
        key.size = jwe->key_len/2;
        cipher_cbc = 1;
      } else {
        key.data = jwe->key;
        key.size = jwe->key_len;
        cipher_cbc = 0;
      }
      iv.data = jwe->iv;
      iv.size = jwe->iv_len;
      payload_enc_len = ciphertext_len;
      if (!(res = gnutls_cipher_init(&handle, _r_get_alg_from_enc(jwe->enc), &key, &iv))) {
        if (jwe->aad_b64url == NULL || jwe->token_mode == R_JSON_MODE_COMPACT) {
          aad = (unsigned char *)o_strdup((const char *)jwe->header_b64url);
        } else {
          aad = (unsigned char *)msprintf("%s.%s", jwe->header_b64url, jwe->aad_b64url);
        }
        if (!cipher_cbc && (res = gnutls_cipher_add_auth(handle, aad, o_strlen((const char *)aad)))) {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_decrypt_payload - Error gnutls_cipher_add_auth: '%s'", gnutls_strerror(res));
          ret = RHN_ERROR;
        }
        if (!(res = gnutls_cipher_decrypt2(handle, ciphertext, ciphertext_len, payload_enc, payload_enc_len))) {
          if (cipher_cbc) {
            r_jwe_remove_padding(payload_enc, &payload_enc_len, gnutls_cipher_get_block_size(_r_get_alg_from_enc(jwe->enc)));
          }
          if (0 == o_strcmp("DEF", r_jwe_get_header_str_value(jwe, "zip"))) {
            if (_r_inflate_payload(payload_enc, payload_enc_len, &unzip, &unzip_len) == RHN_OK) {
              if (r_jwe_set_payload(jwe, unzip, unzip_len) != RHN_OK) {
                y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_decrypt_payload - Error r_jwe_set_payload");
                ret = RHN_ERROR;
              }
            } else {
              y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_decrypt_payload - Error _r_inflate_payload");
              ret = RHN_ERROR;
            }
            o_free(unzip);
          } else {
            if (r_jwe_set_payload(jwe, payload_enc, payload_enc_len) != RHN_OK) {
              y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_decrypt_payload - Error r_jwe_set_payload");
              ret = RHN_ERROR;
            }
          }
        } else if (res == GNUTLS_E_DECRYPTION_FAILED) {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_decrypt_payload - decryption failed: '%s'", gnutls_strerror(res));
          ret = RHN_ERROR_INVALID;
        } else {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_decrypt_payload - Error gnutls_cipher_decrypt: '%s'", gnutls_strerror(res));
          ret = RHN_ERROR;
        }
        if (ret == RHN_OK) {
          if (cipher_cbc) {
            if (r_jwe_compute_hmac_tag(jwe, ciphertext, ciphertext_len, aad, tag, &tag_len) != RHN_OK) {
              y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_decrypt_payload - Error r_jwe_compute_hmac_tag");
              ret = RHN_ERROR;
            }
          } else {
            tag_len = gnutls_cipher_get_tag_size(_r_get_alg_from_enc(jwe->enc));
            memset(tag, 0, tag_len);
            if ((res = gnutls_cipher_tag(handle, tag, tag_len))) {
              y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_decrypt_payload - Error gnutls_cipher_tag: '%s'", gnutls_strerror(res));
              ret = RHN_ERROR;
            }
          }
          if (ret == RHN_OK && tag_len) {
            if ((tag_b64url = o_malloc(tag_len*2)) != NULL) {
              if (o_base64url_encode(tag, tag_len, tag_b64url, &tag_b64url_len)) {
                if (tag_b64url_len != o_strlen((const char *)jwe->auth_tag_b64url) || 0 != memcmp(tag_b64url, jwe->auth_tag_b64url, tag_b64url_len)) {
                  y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_decrypt_payload - Invalid tag");
                  ret = RHN_ERROR_INVALID;
                }
              } else {
                y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_decrypt_payload - Error o_base64url_encode tag_b64url");
                ret = RHN_ERROR;
              }
              o_free(tag_b64url);
            } else {
              y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_decrypt_payload - Error allocating resources for tag_b64url");
              ret = RHN_ERROR_MEMORY;
            }
          }
        }
        o_free(aad);
        gnutls_cipher_deinit(handle);
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_decrypt_payload - Error gnutls_cipher_init: '%s'", gnutls_strerror(res));
        ret = RHN_ERROR;
      }
    }
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_decrypt_payload - Error input parameters");
    ret = RHN_ERROR_PARAM;
  }
  o_free(payload_enc);
  o_free(ciphertext);

  return ret;
}