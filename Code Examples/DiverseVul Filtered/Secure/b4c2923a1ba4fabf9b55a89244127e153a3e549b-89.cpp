int r_jwe_encrypt_key(jwe_t * jwe, jwk_t * jwk_s, int x5u_flags) {
  int ret, res = RHN_OK;
  jwk_t * jwk = NULL;
  jwa_alg alg;
  const char * kid;
  json_t * j_header = NULL, * j_cur_header = NULL;

  if (jwe != NULL) {
    if (jwk_s != NULL) {
      jwk = r_jwk_copy(jwk_s);
      if (jwe->alg == R_JWA_ALG_UNKNOWN && (alg = r_str_to_jwa_alg(r_jwk_get_property_str(jwk, "alg"))) != R_JWA_ALG_NONE) {
        r_jwe_set_alg(jwe, alg);
      }
    } else {
      if (r_jwe_get_header_str_value(jwe, "kid") != NULL) {
        jwk = r_jwks_get_by_kid(jwe->jwks_pubkey, r_jwe_get_header_str_value(jwe, "kid"));
      } else if (r_jwks_size(jwe->jwks_pubkey) == 1) {
        jwk = r_jwks_get_at(jwe->jwks_pubkey, 0);
      }
    }
  }

  if (jwe != NULL && jwe->key != NULL && jwe->key_len && jwe->alg != R_JWA_ALG_UNKNOWN && jwe->alg != R_JWA_ALG_NONE) {
    if ((kid = r_jwk_get_property_str(jwk, "kid")) != NULL && r_jwe_get_header_str_value(jwe, "kid") == NULL) {
      r_jwe_set_header_str_value(jwe, "kid", kid);
    }
    if ((j_header = r_jwe_perform_key_encryption(jwe, jwe->alg, jwk, x5u_flags, &res)) != NULL) {
      j_cur_header = r_jwe_get_full_header_json_t(jwe);
      json_object_update(j_cur_header, json_object_get(j_header, "header"));
      r_jwe_set_full_header_json_t(jwe, j_cur_header);
      json_decref(j_cur_header);
      o_free(jwe->encrypted_key_b64url);
      jwe->encrypted_key_b64url = (unsigned char *)o_strdup(json_string_value(json_object_get(j_header, "encrypted_key")));
      json_decref(j_header);
      ret = RHN_OK;
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_encrypt_key - Error r_jwe_perform_key_encryption");
      ret = res;
    }
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_encrypt_key - invalid input parameters");
    ret = RHN_ERROR_PARAM;
  }

  r_jwk_free(jwk);
  return ret;
}