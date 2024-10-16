json_t * r_jwe_serialize_json_t(jwe_t * jwe, jwks_t * jwks_pubkey, int x5u_flags, int mode) {
  json_t * j_return = NULL, * j_result;
  jwk_t * jwk = NULL;
  jwa_alg alg = R_JWA_ALG_NONE;
  const char * kid = NULL;
  size_t i = 0;
  int res = RHN_OK;

  if (jwks_pubkey == NULL) {
    jwks_pubkey = jwe->jwks_pubkey;
  }
  if (jwe != NULL && r_jwks_size(jwks_pubkey)) {
    jwe->token_mode = mode;
    if (mode == R_JSON_MODE_FLATTENED) {
      if ((kid = r_jwe_get_header_str_value(jwe, "kid")) != NULL) {
        jwk = r_jwks_get_by_kid(jwks_pubkey, kid);
      } else {
        jwk = r_jwks_get_at(jwks_pubkey, 0);
        kid = r_jwk_get_property_str(jwk, "kid");
      }
      alg = r_str_to_jwa_alg(r_jwk_get_property_str(jwk, "alg"));
      if (alg == R_JWA_ALG_UNKNOWN) {
        alg = jwe->alg;
      }
      if (jwe->key == NULL || !jwe->key_len) {
        if (r_jwe_generate_cypher_key(jwe) != RHN_OK) {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_serialize_json_t - Error r_jwe_generate_cypher_key");
          res = RHN_ERROR;
        }
      }
      if (jwe->iv == NULL || !jwe->iv_len) {
        if (r_jwe_generate_iv(jwe) != RHN_OK) {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_serialize_json_t - Error r_jwe_generate_iv");
          res = RHN_ERROR;
        }
      }
      if (res == RHN_OK) {
        if ((j_result = r_jwe_perform_key_encryption(jwe, alg, jwk, x5u_flags, &res)) != NULL) {
          if (r_jwe_encrypt_payload(jwe) == RHN_OK) {
            if ((kid = r_jwe_get_header_str_value(jwe, "kid")) == NULL) {
              kid = r_jwk_get_property_str(jwk, "kid");
            }
            j_return = json_pack("{ss sO* ss ss ss sO*}", "protected", jwe->header_b64url,
                                                          "encrypted_key", json_object_get(j_result, "encrypted_key"),
                                                          "iv", jwe->iv_b64url,
                                                          "ciphertext", jwe->ciphertext_b64url,
                                                          "tag", jwe->auth_tag_b64url,
                                                          "header", json_object_get(j_result, "header"));
            if (jwe->aad_b64url != NULL) {
              json_object_set_new(j_return, "aad", json_string((const char *)jwe->aad_b64url));
            }
            if (jwe->j_unprotected_header != NULL) {
              json_object_set_new(j_return, "unprotected", json_deep_copy(jwe->j_unprotected_header));
            }
            if (kid != NULL) {
              json_object_set_new(json_object_get(j_return, "header"), "kid", json_string(kid));
            }
          } else {
            y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_serialize_json_t - Error input parameters");
          }
        } else {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_serialize_json_t - Error invalid encryption key");
        }
        json_decref(j_result);
      }
      r_jwk_free(jwk);
    } else if (mode == R_JSON_MODE_GENERAL) {
      if (jwe->key == NULL || !jwe->key_len) {
        if (r_jwe_generate_cypher_key(jwe) != RHN_OK) {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_serialize_json_t - Error r_jwe_generate_cypher_key");
          res = RHN_ERROR;
        }
      }
      if (jwe->iv == NULL || !jwe->iv_len) {
        if (r_jwe_generate_iv(jwe) != RHN_OK) {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_serialize_json_t - Error r_jwe_generate_iv");
          res = RHN_ERROR;
        }
      }
      if (res == RHN_OK && r_jwe_encrypt_payload(jwe) == RHN_OK) {
        j_return = json_pack("{ss ss ss ss s[]}", "protected", jwe->header_b64url,
                                              "iv", jwe->iv_b64url,
                                              "ciphertext", jwe->ciphertext_b64url,
                                              "tag", jwe->auth_tag_b64url,
                                              "recipients");
        if (jwe->aad_b64url != NULL) {
          json_object_set_new(j_return, "aad", json_string((const char *)jwe->aad_b64url));
        }
        if (jwe->j_unprotected_header != NULL) {
          json_object_set_new(j_return, "unprotected", json_deep_copy(jwe->j_unprotected_header));
        }
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_serialize_json_t - Error input parameters");
      }
      //r_jwe_set_header_str_value(jwe, "alg", NULL);
      for (i=0; i<r_jwks_size(jwks_pubkey); i++) {
        jwk = r_jwks_get_at(jwks_pubkey, i);
        kid = r_jwk_get_property_str(jwk, "kid");
        if ((alg = r_jwe_get_alg(jwe)) == R_JWA_ALG_UNKNOWN || alg == R_JWA_ALG_NONE) {
          alg = r_str_to_jwa_alg(r_jwk_get_property_str(jwk, "alg"));
        }
        if (alg != R_JWA_ALG_UNKNOWN && alg != R_JWA_ALG_ECDH_ES) {
          if ((j_result = r_jwe_perform_key_encryption(jwe, alg, jwk, x5u_flags, &res)) != NULL) {
            if (json_object_get(jwe->j_header, "kid") == NULL && json_object_get(jwe->j_unprotected_header, "kid") == NULL) {
              json_object_set_new(json_object_get(j_result, "header"), "kid", json_string(r_jwk_get_property_str(jwk, "kid")));
            }
            json_array_append(json_object_get(j_return, "recipients"), j_result);
          } else {
            y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_serialize_json_t - Error invalid encryption key at index %zu", i);
          }
          json_decref(j_result);
        } else if (alg == R_JWA_ALG_ECDH_ES) {
          y_log_message(Y_LOG_LEVEL_DEBUG, "r_jwe_serialize_json_t - Unsupported algorithm for JWE with multiple recipients");
        } else {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_serialize_json_t - Error invalid encryption algorithm at index %zu", i);
        }
        r_jwk_free(jwk);
      }
      if (!json_array_size(json_object_get(j_return, "recipients"))) {
        json_decref(j_return);
        j_return = NULL;
      }
    }
    json_decref(jwe->j_json_serialization);
    jwe->j_json_serialization = json_deep_copy(j_return);
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_serialize_json_t - Error input parameters");
  }
  return j_return;
}