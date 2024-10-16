static int _r_concat_kdf(jwe_t * jwe, jwa_alg alg, const gnutls_datum_t * Z, gnutls_datum_t * kdf) {
  int ret = RHN_OK;
  unsigned char * apu_dec = NULL, * apv_dec = NULL;
  const char * alg_id = alg==R_JWA_ALG_ECDH_ES?r_jwa_enc_to_str(jwe->enc):r_jwa_alg_to_str(alg),
             * apu = r_jwe_get_header_str_value(jwe, "apu"),
             * apv = r_jwe_get_header_str_value(jwe, "apv");
  size_t apu_dec_len = 0, apv_dec_len = 0, alg_id_len = o_strlen(alg_id), key_data_len = 0;

  kdf->data = NULL;
  kdf->size = 0;
  do {
    if ((kdf->data = o_malloc(4+Z->size)) == NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "_r_concat_kdf - Error malloc kdf->data");
      ret = RHN_ERROR_MEMORY;
      break;
    }

    memset(kdf->data, 0, 3);
    memset(kdf->data+3, 1, 1);
    memcpy(kdf->data+4, Z->data, Z->size);
    kdf->size = 4+Z->size;

    if ((kdf->data = o_realloc(kdf->data, kdf->size+4+alg_id_len)) == NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "_r_concat_kdf - Error realloc kdf->data (1)");
      ret = RHN_ERROR_MEMORY;
      break;
    }

    memset(kdf->data+kdf->size, 0, 3);
    memset(kdf->data+kdf->size+3, (uint8_t)alg_id_len, 1);
    memcpy(kdf->data+kdf->size+4, alg_id, alg_id_len);
    kdf->size += 4+alg_id_len;

    if (!o_strnullempty(apu)) {
      if ((apu_dec = o_malloc(o_strlen(apu))) == NULL) {
        y_log_message(Y_LOG_LEVEL_ERROR, "_r_concat_kdf - Error malloc apu_dec");
        ret = RHN_ERROR_MEMORY;
        break;
      }

      if (!o_base64url_decode((const unsigned char *)apu, o_strlen(apu), apu_dec, &apu_dec_len)) {
        y_log_message(Y_LOG_LEVEL_ERROR, "_r_concat_kdf - Error o_base64url_decode apu");
        ret = RHN_ERROR;
        break;
      }
    }

    if ((kdf->data = o_realloc(kdf->data, kdf->size+4+apu_dec_len)) == NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "_r_concat_kdf - Error realloc kdf->data (2)");
      ret = RHN_ERROR_MEMORY;
      break;
    }

    kdf->data[kdf->size] = (unsigned char)(apu_dec_len>>24) & 0xFF;
    kdf->data[kdf->size+1] = (unsigned char)(apu_dec_len>>16) & 0xFF;
    kdf->data[kdf->size+2] = (unsigned char)(apu_dec_len>>8) & 0xFF;
    kdf->data[kdf->size+3] = (unsigned char)(apu_dec_len) & 0xFF;
    if (apu_dec_len) {
      memcpy(kdf->data+kdf->size+4, apu_dec, apu_dec_len);
    }
    kdf->size += apu_dec_len+4;

    if (!o_strnullempty(apv)) {
      if ((apv_dec = o_malloc(o_strlen(apv))) == NULL) {
        y_log_message(Y_LOG_LEVEL_ERROR, "_r_concat_kdf - Error malloc apv_dec");
        ret = RHN_ERROR_MEMORY;
        break;
      }

      if (!o_base64url_decode((const unsigned char *)apv, o_strlen(apv), apv_dec, &apv_dec_len)) {
        y_log_message(Y_LOG_LEVEL_ERROR, "_r_concat_kdf - Error o_base64url_decode apv");
        ret = RHN_ERROR;
        break;
      }
    }

    if ((kdf->data = o_realloc(kdf->data, kdf->size+4+apv_dec_len)) == NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "_r_concat_kdf - Error realloc kdf->data (3)");
      ret = RHN_ERROR_MEMORY;
      break;
    }

    kdf->data[kdf->size] = (unsigned char)(apv_dec_len>>24) & 0xFF;
    kdf->data[kdf->size+1] = (unsigned char)(apv_dec_len>>16) & 0xFF;
    kdf->data[kdf->size+2] = (unsigned char)(apv_dec_len>>8) & 0xFF;
    kdf->data[kdf->size+3] = (unsigned char)(apv_dec_len) & 0xFF;
    if (apv_dec_len) {
      memcpy(kdf->data+kdf->size+4, apv_dec, apv_dec_len);
    }
    kdf->size += apv_dec_len+4;

    if (alg == R_JWA_ALG_ECDH_ES) {
      key_data_len = _r_get_key_size(jwe->enc)*8;
    } else if (alg == R_JWA_ALG_ECDH_ES_A128KW) {
      key_data_len = 16*8;
    } else if (alg == R_JWA_ALG_ECDH_ES_A192KW) {
      key_data_len = 24*8;
    } else if (alg == R_JWA_ALG_ECDH_ES_A256KW) {
      key_data_len = 32*8;
    }

    if (!key_data_len) {
      y_log_message(Y_LOG_LEVEL_ERROR, "_r_concat_kdf - Error invalid keydatalen");
      ret = RHN_ERROR;
      break;
    }

    if ((kdf->data = o_realloc(kdf->data, kdf->size+4)) == NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "_r_concat_kdf - Error realloc kdf->data (4)");
      ret = RHN_ERROR_MEMORY;
      break;
    }

    kdf->data[kdf->size] = (unsigned char)(key_data_len>>24) & 0xFF;
    kdf->data[kdf->size+1] = (unsigned char)(key_data_len>>16) & 0xFF;
    kdf->data[kdf->size+2] = (unsigned char)(key_data_len>>8) & 0xFF;
    kdf->data[kdf->size+3] = (unsigned char)(key_data_len) & 0xFF;
    kdf->size += 4;

  } while (0);

  o_free(apu_dec);
  o_free(apv_dec);

  if (ret != RHN_OK) {
    o_free(kdf->data);
    kdf->data = NULL;
    kdf->size = 0;
  }

  return ret;
}