static json_t * _r_jwe_ecdh_encrypt(jwe_t * jwe, jwa_alg alg, jwk_t * jwk_pub, jwk_t * jwk_priv, int type, unsigned int bits, int x5u_flags, int * ret) {
  int type_priv = 0;
  unsigned int bits_priv = 0;
  jwk_t * jwk_ephemeral = NULL, * jwk_ephemeral_pub = NULL;
  gnutls_datum_t Z = {NULL, 0}, kdf = {NULL, 0};
  unsigned char cipherkey_b64url[256] = {0};
  uint8_t derived_key[64] = {0}, wrapped_key[72] = {0}, priv_k[_R_CURVE_MAX_SIZE] = {0}, pub_x[_R_CURVE_MAX_SIZE] = {0}, pub_y[_R_CURVE_MAX_SIZE] = {0};
  size_t derived_key_len = 0, cipherkey_b64url_len = 0, priv_k_size = 0, pub_x_size = 0, pub_y_size = 0, crv_size = 0;
  const char * key = NULL;
  json_t * j_return = NULL;
  const struct ecc_curve * nettle_curve;

  do {
    if (r_jwk_init(&jwk_ephemeral_pub) != RHN_OK) {
      y_log_message(Y_LOG_LEVEL_ERROR, "_r_jwe_ecdh_encrypt - Error r_jwk_init jwk_ephemeral_pub");
      *ret = RHN_ERROR;
      break;
    }

    if (jwk_priv != NULL) {
      type_priv = r_jwk_key_type(jwk_priv, &bits_priv, x5u_flags);

      if ((type_priv & 0xffffff00) != (type & 0xffffff00)) {
        y_log_message(Y_LOG_LEVEL_ERROR, "_r_jwe_ecdh_encrypt - Error invalid ephemeral key");
        *ret = RHN_ERROR_PARAM;
        break;
      }

      if (bits != bits_priv) {
        y_log_message(Y_LOG_LEVEL_ERROR, "_r_jwe_ecdh_encrypt - Error invalid ephemeral key length");
        *ret = RHN_ERROR_PARAM;
        break;
      }

      if (r_jwk_extract_pubkey(jwk_priv, jwk_ephemeral_pub, x5u_flags) != RHN_OK) {
        y_log_message(Y_LOG_LEVEL_ERROR, "_r_jwe_ecdh_encrypt - Error extracting public key from jwk_priv");
        *ret = RHN_ERROR;
        break;
      }
    } else {
      if (r_jwk_init(&jwk_ephemeral) != RHN_OK) {
        y_log_message(Y_LOG_LEVEL_ERROR, "_r_jwe_ecdh_encrypt - Error r_jwk_init jwk_ephemeral");
        *ret = RHN_ERROR;
        break;
      }

      if (r_jwk_generate_key_pair(jwk_ephemeral, jwk_ephemeral_pub, type&R_KEY_TYPE_EC?R_KEY_TYPE_EC:R_KEY_TYPE_ECDH, bits, NULL) != RHN_OK) {
        y_log_message(Y_LOG_LEVEL_ERROR, "_r_jwe_ecdh_encrypt - Error r_jwk_generate_key_pair");
        *ret = RHN_ERROR;
        break;
      }

      r_jwk_delete_property_str(jwk_ephemeral_pub, "kid");
    }

    if (type & R_KEY_TYPE_EC) {
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

      if (jwk_priv != NULL) {
        key = r_jwk_get_property_str(jwk_priv, "d");
      } else {
        key = r_jwk_get_property_str(jwk_ephemeral, "d");
      }
      if (!o_base64url_decode((const unsigned char *)key, o_strlen(key), NULL, &priv_k_size)) {
        y_log_message(Y_LOG_LEVEL_ERROR, "_r_jwe_ecdh_encrypt - Error o_base64url_decode d (ecdsa)");
        *ret = RHN_ERROR_PARAM;
        break;
      }

      if (!priv_k_size || priv_k_size > _R_CURVE_MAX_SIZE) {
        y_log_message(Y_LOG_LEVEL_ERROR, "_r_jwe_ecdh_encrypt - Invalid priv_k_size (ecdsa)");
        *ret = RHN_ERROR_PARAM;
        break;
      }

      if (!o_base64url_decode((const unsigned char *)key, o_strlen(key), priv_k, &priv_k_size)) {
        y_log_message(Y_LOG_LEVEL_ERROR, "_r_jwe_ecdh_encrypt - Error o_base64url_decode d (ecdsa)");
        *ret = RHN_ERROR_PARAM;
        break;
      }

      key = r_jwk_get_property_str(jwk_pub, "x");
      if (!o_base64url_decode((const unsigned char *)key, o_strlen(key), NULL, &pub_x_size)) {
        y_log_message(Y_LOG_LEVEL_ERROR, "_r_jwe_ecdh_encrypt - Error o_base64url_decode x (ecdsa)");
        *ret = RHN_ERROR_PARAM;
        break;
      }

      if (!pub_x_size || pub_x_size > _R_CURVE_MAX_SIZE) {
        y_log_message(Y_LOG_LEVEL_ERROR, "_r_jwe_ecdh_encrypt - Invalid pub_x_size (ecdsa)");
        *ret = RHN_ERROR_PARAM;
        break;
      }

      if (!o_base64url_decode((const unsigned char *)key, o_strlen(key), pub_x, &pub_x_size)) {
        y_log_message(Y_LOG_LEVEL_ERROR, "_r_jwe_ecdh_encrypt - Error o_base64url_decode x (ecdsa)");
        *ret = RHN_ERROR_PARAM;
        break;
      }

      key = r_jwk_get_property_str(jwk_pub, "y");
      if (!o_base64url_decode((const unsigned char *)key, o_strlen(key), NULL, &pub_y_size)) {
        y_log_message(Y_LOG_LEVEL_ERROR, "_r_jwe_ecdh_encrypt - Error o_base64url_decode y (ecdsa)");
        *ret = RHN_ERROR_PARAM;
        break;
      }

      if (!pub_y_size || pub_y_size > _R_CURVE_MAX_SIZE) {
        y_log_message(Y_LOG_LEVEL_ERROR, "_r_jwe_ecdh_encrypt - Invalid pub_y_size (ecdsa)");
        *ret = RHN_ERROR_PARAM;
        break;
      }

      if (!o_base64url_decode((const unsigned char *)key, o_strlen(key), pub_y, &pub_y_size)) {
        y_log_message(Y_LOG_LEVEL_ERROR, "_r_jwe_ecdh_encrypt - Error o_base64url_decode y (ecdsa)");
        *ret = RHN_ERROR_PARAM;
        break;
      }

      if (_r_ecdh_compute(priv_k, priv_k_size, pub_x, pub_x_size, pub_y, pub_y_size, nettle_curve, &Z) != RHN_OK) {
        y_log_message(Y_LOG_LEVEL_ERROR, "_r_jwe_ecdh_encrypt - Error _r_ecdh_compute (ecdsa)");
        *ret = RHN_ERROR;
        break;
      }
    } else {
      if (bits == 256) {
        crv_size = CURVE25519_SIZE;
      } else {
        crv_size = CURVE448_SIZE;
      }

      if (jwk_priv != NULL) {
        key = r_jwk_get_property_str(jwk_priv, "d");
      } else {
        key = r_jwk_get_property_str(jwk_ephemeral, "d");
      }
      if (!o_base64url_decode((const unsigned char *)key, o_strlen(key), NULL, &priv_k_size)) {
        y_log_message(Y_LOG_LEVEL_ERROR, "_r_jwe_ecdh_encrypt - Error o_base64url_decode d (eddsa)");
        *ret = RHN_ERROR_PARAM;
        break;
      }

      if (!priv_k_size || priv_k_size > _R_CURVE_MAX_SIZE) {
        y_log_message(Y_LOG_LEVEL_ERROR, "_r_jwe_ecdh_encrypt - Invalid priv_k_size (eddsa)");
        *ret = RHN_ERROR_PARAM;
        break;
      }

      if (!o_base64url_decode((const unsigned char *)key, o_strlen(key), priv_k, &priv_k_size)) {
        y_log_message(Y_LOG_LEVEL_ERROR, "_r_jwe_ecdh_encrypt - Error o_base64url_decode d (eddsa)");
        *ret = RHN_ERROR_PARAM;
        break;
      }

      pub_x_size = CURVE448_SIZE;
      key = r_jwk_get_property_str(jwk_pub, "x");
      if (!o_base64url_decode((const unsigned char *)key, o_strlen(key), NULL, &pub_x_size)) {
        y_log_message(Y_LOG_LEVEL_ERROR, "_r_jwe_ecdh_encrypt - Error o_base64url_decode x (eddsa)");
        *ret = RHN_ERROR_PARAM;
        break;
      }

      if (!pub_x_size || pub_x_size > _R_CURVE_MAX_SIZE) {
        y_log_message(Y_LOG_LEVEL_ERROR, "_r_jwe_ecdh_encrypt - Invalid pub_x_size (eddsa)");
        *ret = RHN_ERROR_PARAM;
        break;
      }

      if (!o_base64url_decode((const unsigned char *)key, o_strlen(key), pub_x, &pub_x_size)) {
        y_log_message(Y_LOG_LEVEL_ERROR, "_r_jwe_ecdh_encrypt - Error o_base64url_decode x (eddsa)");
        *ret = RHN_ERROR_PARAM;
        break;
      }

      if (_r_dh_compute(priv_k, pub_x, crv_size, &Z) != GNUTLS_E_SUCCESS) {
        y_log_message(Y_LOG_LEVEL_ERROR, "_r_jwe_ecdh_encrypt - Error _r_dh_compute (eddsa)");
        *ret = RHN_ERROR;
        break;
      }
    }


    if (_r_concat_kdf(jwe, alg, &Z, &kdf) != RHN_OK) {
      y_log_message(Y_LOG_LEVEL_ERROR, "_r_jwe_ecdh_encrypt - Error _r_concat_kdf");
      *ret = RHN_ERROR;
      break;
    }

    if (gnutls_hash_fast(GNUTLS_DIG_SHA256, kdf.data, kdf.size, derived_key) != GNUTLS_E_SUCCESS) {
      y_log_message(Y_LOG_LEVEL_ERROR, "_r_jwe_ecdh_encrypt - Error gnutls_hash_fast");
      *ret = RHN_ERROR;
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
      o_free(jwe->encrypted_key_b64url);
      jwe->encrypted_key_b64url = NULL;
      j_return = json_pack("{s{ss so}}", "header",
                                           "alg", r_jwa_alg_to_str(alg),
                                           "epk", r_jwk_export_to_json_t(jwk_ephemeral_pub));
    } else {
      _r_aes_key_wrap(derived_key, derived_key_len, jwe->key, jwe->key_len, wrapped_key);
      if (!o_base64url_encode(wrapped_key, jwe->key_len+8, cipherkey_b64url, &cipherkey_b64url_len)) {
        y_log_message(Y_LOG_LEVEL_ERROR, "_r_jwe_ecdh_encrypt - Error o_base64url_encode wrapped_key");
        *ret = RHN_ERROR;
      }
      o_free(jwe->encrypted_key_b64url);
      jwe->encrypted_key_b64url = (unsigned char *)o_strndup((const char *)cipherkey_b64url, cipherkey_b64url_len);
      j_return = json_pack("{ss%s{ss so}}", "encrypted_key", cipherkey_b64url, cipherkey_b64url_len,
                                             "header",
                                               "alg", r_jwa_alg_to_str(alg),
                                               "epk", r_jwk_export_to_json_t(jwk_ephemeral_pub));
    }
  } while (0);

  o_free(kdf.data);
  gnutls_free(Z.data);
  r_jwk_free(jwk_ephemeral);
  r_jwk_free(jwk_ephemeral_pub);

  return j_return;
}