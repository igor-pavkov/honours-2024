int r_jwe_decrypt(jwe_t * jwe, jwk_t * jwk_privkey, int x5u_flags) {
  int ret, res;
  json_t * j_recipient = NULL, * j_header, * j_cur_header;
  size_t index = 0, i;
  jwk_t * jwk = NULL, * cur_jwk = NULL;
  jwa_alg alg;

  if (jwe != NULL) {
    if (jwk_privkey != NULL) {
      jwk = r_jwk_copy(jwk_privkey);
    } else {
      if (r_jwe_get_header_str_value(jwe, "kid") != NULL) {
        jwk = r_jwks_get_by_kid(jwe->jwks_privkey, r_jwe_get_header_str_value(jwe, "kid"));
      } else if (r_jwks_size(jwe->jwks_privkey) == 1) {
        jwk = r_jwks_get_at(jwe->jwks_privkey, 0);
      }
    }
  }

  if (jwe != NULL) {
    if (jwe->token_mode == R_JSON_MODE_GENERAL) {
      ret = RHN_ERROR_INVALID;
      o_free(jwe->encrypted_key_b64url);
      j_header = r_jwe_get_full_header_json_t(jwe);
      json_array_foreach(json_object_get(jwe->j_json_serialization, "recipients"), index, j_recipient) {
        j_cur_header = json_deep_copy(j_header);
        json_object_update(j_cur_header, json_object_get(j_recipient, "header"));
        r_jwe_set_full_header_json_t(jwe, j_cur_header);
        json_decref(j_cur_header);
        jwe->encrypted_key_b64url = (unsigned char *)json_string_value(json_object_get(j_recipient, "encrypted_key"));
        alg = r_jwe_get_alg(jwe);
        if (json_object_get(jwe->j_unprotected_header, "alg") != NULL) {
          alg = r_str_to_jwa_alg(json_string_value(json_object_get(jwe->j_unprotected_header, "alg")));
        }
        if (json_object_get(json_object_get(j_recipient, "header"), "alg") != NULL) {
          alg = r_str_to_jwa_alg(json_string_value(json_object_get(json_object_get(j_recipient, "header"), "alg")));
        }
        if (alg != R_JWA_ALG_UNKNOWN && alg != R_JWA_ALG_ECDH_ES) {
          if (jwk_privkey != NULL) {
            if (r_jwk_get_property_str(jwk_privkey, "kid") == NULL || json_object_get(json_object_get(j_recipient, "header"), "kid") == NULL || 0 == o_strcmp(json_string_value(json_object_get(json_object_get(j_recipient, "header"), "kid")), r_jwk_get_property_str(jwk_privkey, "kid"))) {
              if ((res = r_preform_key_decryption(jwe, alg, jwk_privkey, x5u_flags)) != RHN_ERROR_INVALID) {
                ret = res;
                break;
              }
            }
          } else {
            if (json_object_get(json_object_get(j_recipient, "header"), "kid") != NULL) {
              cur_jwk = r_jwks_get_by_kid(jwe->jwks_privkey, json_string_value(json_object_get(json_object_get(j_recipient, "header"), "kid")));
              if ((res = r_preform_key_decryption(jwe, alg, cur_jwk, x5u_flags)) != RHN_ERROR_INVALID) {
                ret = res;
                r_jwk_free(cur_jwk);
                break;
              }
              r_jwk_free(cur_jwk);
            } else {
              for (i=0; i<r_jwks_size(jwe->jwks_privkey); i++) {
                cur_jwk = r_jwks_get_at(jwe->jwks_privkey, i);
                if ((res = r_preform_key_decryption(jwe, alg, cur_jwk, x5u_flags)) != RHN_ERROR_INVALID) {
                  ret = res;
                  r_jwk_free(cur_jwk);
                  break;
                }
                r_jwk_free(cur_jwk);
              }
              if (ret != RHN_ERROR_INVALID) {
                break;
              }
            }
          }
          cur_jwk = NULL;
        } else if (alg == R_JWA_ALG_ECDH_ES) {
          y_log_message(Y_LOG_LEVEL_DEBUG, "r_jwe_decrypt - Unsupported algorithm ECDH-ES on general serialization");
        } else {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_decrypt - Invalid alg value at index %zu: %d", index, (alg));
          ret = RHN_ERROR_PARAM;
        }
      }
      r_jwe_set_full_header_json_t(jwe, j_header);
      json_decref(j_header);
      jwe->encrypted_key_b64url = NULL;
      if (ret == RHN_OK) {
        ret = r_jwe_decrypt_payload(jwe);
      }
    } else {
      j_header = r_jwe_get_full_header_json_t(jwe);
      j_cur_header = json_deep_copy(j_header);
      json_object_update(j_cur_header, json_object_get(j_recipient, "header"));
      if (jwe->j_unprotected_header != NULL) {
        json_object_update(j_cur_header, jwe->j_unprotected_header);
      }
      r_jwe_set_full_header_json_t(jwe, j_cur_header);
      json_decref(j_cur_header);
      if ((res = r_jwe_decrypt_key(jwe, jwk, x5u_flags)) == RHN_OK && (res = r_jwe_decrypt_payload(jwe)) == RHN_OK) {
        ret = RHN_OK;
      } else {
        if (res != RHN_ERROR_INVALID) {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_decrypt - Error decrypting data");
        }
        ret = res;
      }
      r_jwe_set_full_header_json_t(jwe, j_header);
      json_decref(j_header);
    }
  } else {
    ret = RHN_ERROR_PARAM;
  }
  r_jwk_free(jwk);
  return ret;
}