static int _r_jwe_ecdh_decrypt(jwe_t * jwe, jwa_alg alg, jwk_t * jwk, int type, unsigned int bits, int x5u_flags) {
  int ret = RHN_OK;
  jwk_t * jwk_ephemeral_pub = NULL;
  json_t * j_epk = NULL;
  unsigned int epk_bits = 0;
  gnutls_datum_t Z = {NULL, 0}, kdf = {NULL, 0};
  uint8_t derived_key[64] = {0}, key_data[72] = {0}, cipherkey[128] = {0}, priv_k[_R_CURVE_MAX_SIZE] = {0}, pub_x[_R_CURVE_MAX_SIZE] = {0}, pub_y[_R_CURVE_MAX_SIZE] = {0};
  size_t derived_key_len = 0, cipherkey_len = 0, priv_k_size = 0, pub_x_size = 0, pub_y_size = 0, crv_size = 0;
  const char * key = NULL;
  const struct ecc_curve * nettle_curve;

  do {
    if ((j_epk = r_jwe_get_header_json_t_value(jwe, "epk")) == NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "_r_jwe_ecdh_decrypt - No epk header");
      ret = RHN_ERROR_PARAM;
      break;
    }

    if (r_jwk_init(&jwk_ephemeral_pub) != RHN_OK) {
      y_log_message(Y_LOG_LEVEL_ERROR, "_r_jwe_ecdh_decrypt - Error r_jwk_init");
      ret = RHN_ERROR;
      break;
    }

    if (r_jwk_import_from_json_t(jwk_ephemeral_pub, j_epk) != RHN_OK) {
      y_log_message(Y_LOG_LEVEL_ERROR, "_r_jwe_ecdh_decrypt - Error r_jwk_import_from_json_t");
      ret = RHN_ERROR_PARAM;
      break;
    }

    if (type & R_KEY_TYPE_EC) {
      if (!(r_jwk_key_type(jwk_ephemeral_pub, &epk_bits, x5u_flags) & (R_KEY_TYPE_EC|R_KEY_TYPE_PUBLIC)) || epk_bits != bits) {
        y_log_message(Y_LOG_LEVEL_ERROR, "_r_jwe_ecdh_decrypt - Error invalid private key type (ecc)");
        ret = RHN_ERROR_PARAM;
        break;
      }

      if (bits == 256) {
        nettle_curve = nettle_get_secp_256r1();
        crv_size = 32;
      } else if (bits == 384) {
        nettle_curve = nettle_get_secp_384r1();
        crv_size = 48;
      } else {
        nettle_curve = nettle_get_secp_521r1();
        crv_size = 64;
      }

      key = r_jwk_get_property_str(jwk, "d");
      if (!o_base64url_decode((const unsigned char *)key, o_strlen(key), NULL, &priv_k_size)) {
        y_log_message(Y_LOG_LEVEL_ERROR, "_r_jwe_ecdh_decrypt - Error o_base64url_decode d (ecdsa)");
        ret = RHN_ERROR_PARAM;
        break;
      }

      if (!priv_k_size || priv_k_size > _R_CURVE_MAX_SIZE) {
        y_log_message(Y_LOG_LEVEL_ERROR, "_r_jwe_ecdh_decrypt - Invalid priv_k_size (ecdsa)");
        ret = RHN_ERROR_PARAM;
        break;
      }

      if (!o_base64url_decode((const unsigned char *)key, o_strlen(key), priv_k, &priv_k_size)) {
        y_log_message(Y_LOG_LEVEL_ERROR, "_r_jwe_ecdh_decrypt - Error o_base64url_decode d (ecdsa)");
        ret = RHN_ERROR_PARAM;
        break;
      }

      key = r_jwk_get_property_str(jwk_ephemeral_pub, "x");
      if (!o_base64url_decode((const unsigned char *)key, o_strlen(key), NULL, &pub_x_size)) {
        y_log_message(Y_LOG_LEVEL_ERROR, "_r_jwe_ecdh_decrypt - Error o_base64url_decode x (ecdsa)");
        ret = RHN_ERROR_PARAM;
        break;
      }

      if (!pub_x_size || pub_x_size > _R_CURVE_MAX_SIZE) {
        y_log_message(Y_LOG_LEVEL_ERROR, "_r_jwe_ecdh_decrypt - Invalid pub_x_size (ecdsa)");
        ret = RHN_ERROR_PARAM;
        break;
      }

      if (!o_base64url_decode((const unsigned char *)key, o_strlen(key), pub_x, &pub_x_size)) {
        y_log_message(Y_LOG_LEVEL_ERROR, "_r_jwe_ecdh_decrypt - Error o_base64url_decode x (ecdsa)");
        ret = RHN_ERROR_PARAM;
        break;
      }

      key = r_jwk_get_property_str(jwk_ephemeral_pub, "y");
      if (!o_base64url_decode((const unsigned char *)key, o_strlen(key), NULL, &pub_y_size)) {
        y_log_message(Y_LOG_LEVEL_ERROR, "_r_jwe_ecdh_decrypt - Error o_base64url_decode y (ecdsa)");
        ret = RHN_ERROR_PARAM;
        break;
      }

      if (!pub_y_size || pub_y_size > _R_CURVE_MAX_SIZE) {
        y_log_message(Y_LOG_LEVEL_ERROR, "_r_jwe_ecdh_decrypt - Invalid pub_y_size (ecdsa)");
        ret = RHN_ERROR_PARAM;
        break;
      }

      if (!o_base64url_decode((const unsigned char *)key, o_strlen(key), pub_y, &pub_y_size)) {
        y_log_message(Y_LOG_LEVEL_ERROR, "_r_jwe_ecdh_decrypt - Error o_base64url_decode y (ecdsa)");
        ret = RHN_ERROR_PARAM;
        break;
      }

      if (_r_ecdh_compute(priv_k, priv_k_size, pub_x, pub_x_size, pub_y, pub_y_size, nettle_curve, &Z) != RHN_OK) {
        y_log_message(Y_LOG_LEVEL_ERROR, "_r_jwe_ecdh_decrypt - Error _r_ecdh_compute (ecdsa)");
        ret = RHN_ERROR;
        break;
      }
    } else {
      if (!(r_jwk_key_type(jwk_ephemeral_pub, &epk_bits, x5u_flags) & (R_KEY_TYPE_ECDH|R_KEY_TYPE_PUBLIC)) || epk_bits != bits) {
        y_log_message(Y_LOG_LEVEL_ERROR, "_r_jwe_ecdh_decrypt - Error invalid private key type (eddsa)");
        ret = RHN_ERROR_INVALID;
        break;
      }

      if (bits == 256) {
        crv_size = CURVE25519_SIZE;
      } else {
        crv_size = CURVE448_SIZE;
      }

      key = r_jwk_get_property_str(jwk, "d");
      if (!o_base64url_decode((const unsigned char *)key, o_strlen(key), priv_k, &priv_k_size)) {
        y_log_message(Y_LOG_LEVEL_ERROR, "_r_jwe_ecdh_decrypt - Error o_base64url_decode d (eddsa)");
        ret = RHN_ERROR_PARAM;
        break;
      }

      if (!priv_k_size || priv_k_size > _R_CURVE_MAX_SIZE) {
        y_log_message(Y_LOG_LEVEL_ERROR, "_r_jwe_ecdh_decrypt - Invalid priv_k_size (eddsa)");
        ret = RHN_ERROR_PARAM;
        break;
      }

      if (!o_base64url_decode((const unsigned char *)key, o_strlen(key), NULL, &priv_k_size)) {
        y_log_message(Y_LOG_LEVEL_ERROR, "_r_jwe_ecdh_decrypt - Error o_base64url_decode d (eddsa)");
        ret = RHN_ERROR_PARAM;
        break;
      }

      key = r_jwk_get_property_str(jwk_ephemeral_pub, "x");
      if (!o_base64url_decode((const unsigned char *)key, o_strlen(key), pub_x, &pub_x_size)) {
        y_log_message(Y_LOG_LEVEL_ERROR, "_r_jwe_ecdh_decrypt - Error o_base64url_decode x (eddsa)");
        ret = RHN_ERROR_PARAM;
        break;
      }

      if (!pub_x_size || pub_x_size > _R_CURVE_MAX_SIZE) {
        y_log_message(Y_LOG_LEVEL_ERROR, "_r_jwe_ecdh_decrypt - Invalid priv_k_size (eddsa)");
        ret = RHN_ERROR_PARAM;
        break;
      }

      if (!o_base64url_decode((const unsigned char *)key, o_strlen(key), NULL, &pub_x_size)) {
        y_log_message(Y_LOG_LEVEL_ERROR, "_r_jwe_ecdh_decrypt - Error o_base64url_decode x (eddsa)");
        ret = RHN_ERROR_PARAM;
        break;
      }

      if (_r_dh_compute(priv_k, pub_x, crv_size, &Z) != GNUTLS_E_SUCCESS) {
        y_log_message(Y_LOG_LEVEL_ERROR, "_r_jwe_ecdh_decrypt - Error _r_dh_compute (eddsa)");
        ret = RHN_ERROR;
        break;
      }
    }

    if (_r_concat_kdf(jwe, alg, &Z, &kdf) != RHN_OK) {
      y_log_message(Y_LOG_LEVEL_ERROR, "_r_jwe_ecdh_decrypt - Error _r_concat_kdf");
      ret = RHN_ERROR;
      break;
    }

    if (gnutls_hash_fast(GNUTLS_DIG_SHA256, kdf.data, kdf.size, derived_key) != GNUTLS_E_SUCCESS) {
      y_log_message(Y_LOG_LEVEL_ERROR, "_r_jwe_ecdh_decrypt - Error gnutls_hash_fast");
      ret = RHN_ERROR;
      break;
    }

    if (alg == R_JWA_ALG_ECDH_ES) {
      derived_key_len = _r_get_key_size(jwe->enc);
    } else if (alg == R_JWA_ALG_ECDH_ES_A128KW) {
      derived_key_len = 16;
    } else if (alg == R_JWA_ALG_ECDH_ES_A192KW) {
      derived_key_len = 24;
    } else if (alg == R_JWA_ALG_ECDH_ES_A256KW) {
      derived_key_len = 32;
    }

    if (alg == R_JWA_ALG_ECDH_ES) {
      r_jwe_set_cypher_key(jwe, derived_key, derived_key_len);
    } else {
      if (o_base64url_decode(jwe->encrypted_key_b64url, o_strlen((const char *)jwe->encrypted_key_b64url), cipherkey, &cipherkey_len)) {
        if (_r_aes_key_unwrap(derived_key, derived_key_len, key_data, cipherkey_len-8, cipherkey)) {
          r_jwe_set_cypher_key(jwe, key_data, cipherkey_len-8);
        } else {
          ret = RHN_ERROR_INVALID;
          break;
        }
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "_r_jwe_ecdh_decrypt - Error o_base64url_decode cipherkey");
        ret = RHN_ERROR;
        break;
      }
    }
  } while (0);

  o_free(kdf.data);
  gnutls_free(Z.data);
  r_jwk_free(jwk_ephemeral_pub);
  json_decref(j_epk);

  return ret;
}