int r_jwe_encrypt_payload(jwe_t * jwe) {
  int ret = RHN_OK, res;
  gnutls_cipher_hd_t handle;
  gnutls_datum_t key, iv;
  unsigned char * ptext = NULL, * text_zip = NULL, * ciphertext_b64url = NULL, tag[128] = {0}, * tag_b64url = NULL, * str_header_b64 = NULL, * aad = NULL;
  size_t ptext_len = 0, ciphertext_b64url_len = 0, tag_len = 0, tag_b64url_len = 0, str_header_b64_len = 0, text_zip_len = 0;
  char * str_header = NULL;
  int cipher_cbc;

  if (jwe != NULL &&
      jwe->payload != NULL &&
      jwe->payload_len &&
      jwe->enc != R_JWA_ENC_UNKNOWN &&
      jwe->key != NULL &&
      jwe->iv != NULL &&
      jwe->iv_len &&
      jwe->key_len == _r_get_key_size(jwe->enc) &&
      r_jwe_set_enc_header(jwe, jwe->j_header) == RHN_OK) {
    cipher_cbc = (jwe->enc == R_JWA_ENC_A128CBC || jwe->enc == R_JWA_ENC_A192CBC || jwe->enc == R_JWA_ENC_A256CBC);

    if ((str_header = json_dumps(jwe->j_header, JSON_COMPACT)) != NULL) {
      if ((str_header_b64 = o_malloc(o_strlen(str_header)*2)) != NULL) {
        if (o_base64url_encode((const unsigned char *)str_header, o_strlen(str_header), str_header_b64, &str_header_b64_len)) {
          o_free(jwe->header_b64url);
          jwe->header_b64url = (unsigned char *)o_strndup((const char *)str_header_b64, str_header_b64_len);
        } else {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_encrypt_payload - Error o_base64url_encode str_header");
          ret = RHN_ERROR;
        }
        o_free(str_header_b64);
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_encrypt_payload - Error allocating resources for str_header_b64");
        ret = RHN_ERROR_MEMORY;
      }
      o_free(str_header);
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_encrypt_payload - Error json_dumps j_header");
      ret = RHN_ERROR;
    }

    ptext_len = gnutls_cipher_get_block_size(_r_get_alg_from_enc(jwe->enc));
    if (0 == o_strcmp("DEF", r_jwe_get_header_str_value(jwe, "zip"))) {
      if (_r_deflate_payload(jwe->payload, jwe->payload_len, &text_zip, &text_zip_len) == RHN_OK) {
        if (r_jwe_set_ptext_with_block(text_zip, text_zip_len, &ptext, &ptext_len, _r_get_alg_from_enc(jwe->enc), cipher_cbc) != RHN_OK) {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_encrypt_payload - Error r_jwe_set_ptext_with_block");
          ret = RHN_ERROR;
        }
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_encrypt_payload - Error _r_deflate_payload");
        ret = RHN_ERROR;
      }
      o_free(text_zip);
    } else {
      if (r_jwe_set_ptext_with_block(jwe->payload, jwe->payload_len, &ptext, &ptext_len, _r_get_alg_from_enc(jwe->enc), cipher_cbc) != RHN_OK) {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_encrypt_payload - Error r_jwe_set_ptext_with_block");
        ret = RHN_ERROR;
      }
    }

    if (ret == RHN_OK) {
      if (cipher_cbc) {
        key.data = jwe->key+(jwe->key_len/2);
        key.size = jwe->key_len/2;
      } else {
        key.data = jwe->key;
        key.size = jwe->key_len;
      }
      iv.data = jwe->iv;
      iv.size = jwe->iv_len;
      if (!(res = gnutls_cipher_init(&handle, _r_get_alg_from_enc(jwe->enc), &key, &iv))) {
        if (jwe->aad_b64url == NULL || jwe->token_mode == R_JSON_MODE_COMPACT) {
          aad = (unsigned char *)o_strdup((const char *)jwe->header_b64url);
        } else {
          aad = (unsigned char *)msprintf("%s.%s", jwe->header_b64url, jwe->aad_b64url);
        }
        if (!cipher_cbc && (res = gnutls_cipher_add_auth(handle, aad, o_strlen((const char *)aad)))) {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_encrypt_payload - Error gnutls_cipher_add_auth: '%s'", gnutls_strerror(res));
          ret = RHN_ERROR;
        }
        if (ret == RHN_OK) {
          if (!(res = gnutls_cipher_encrypt(handle, ptext, ptext_len))) {
            if ((ciphertext_b64url = o_malloc(2*ptext_len)) != NULL) {
              if (o_base64url_encode(ptext, ptext_len, ciphertext_b64url, &ciphertext_b64url_len)) {
                o_free(jwe->ciphertext_b64url);
                jwe->ciphertext_b64url = (unsigned char *)o_strndup((const char *)ciphertext_b64url, ciphertext_b64url_len);
              } else {
                y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_encrypt_payload - Error o_base64url_encode ciphertext");
                ret = RHN_ERROR;
              }
            } else {
              y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_encrypt_payload - Error allocating resources for ciphertext_b64url");
              ret = RHN_ERROR_MEMORY;
            }
            o_free(ciphertext_b64url);
          } else {
            y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_encrypt_payload - Error gnutls_cipher_encrypt: '%s'", gnutls_strerror(res));
            ret = RHN_ERROR;
          }
        } else if (!cipher_cbc) {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_encrypt_payload - Error gnutls_cipher_add_auth: '%s'", gnutls_strerror(res));
          ret = RHN_ERROR;
        }
        if (ret == RHN_OK) {
          if (cipher_cbc) {
            if (r_jwe_compute_hmac_tag(jwe, ptext, ptext_len, aad, tag, &tag_len) != RHN_OK) {
              y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_encrypt_payload - Error r_jwe_compute_hmac_tag");
              ret = RHN_ERROR;
            }
          } else {
            tag_len = gnutls_cipher_get_tag_size(_r_get_alg_from_enc(jwe->enc));
            memset(tag, 0, tag_len);
            if ((res = gnutls_cipher_tag(handle, tag, tag_len))) {
              y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_encrypt_payload - Error gnutls_cipher_tag: '%s'", gnutls_strerror(res));
              ret = RHN_ERROR;
            }
          }
          if (ret == RHN_OK && tag_len) {
            if ((tag_b64url = o_malloc(tag_len*2)) != NULL) {
              if (o_base64url_encode(tag, tag_len, tag_b64url, &tag_b64url_len)) {
                o_free(jwe->auth_tag_b64url);
                jwe->auth_tag_b64url = (unsigned char *)o_strndup((const char *)tag_b64url, tag_b64url_len);
              } else {
                y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_encrypt_payload - Error o_base64url_encode tag_b64url");
                ret = RHN_ERROR;
              }
              o_free(tag_b64url);
            } else {
              y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_encrypt_payload - Error allocating resources for tag_b64url");
              ret = RHN_ERROR_MEMORY;
            }
          }
        }
        o_free(aad);
        gnutls_cipher_deinit(handle);
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_encrypt_payload - Error gnutls_cipher_init: '%s'", gnutls_strerror(res));
        ret = RHN_ERROR;
      }
    }
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_encrypt_payload - Error input parameters");
    ret = RHN_ERROR_PARAM;
  }
  o_free(ptext);
  return ret;
}