int r_jwe_set_full_header_json_t(jwe_t * jwe, json_t * j_header) {
  int ret = RHN_OK;
  jwa_alg alg;
  jwa_enc enc;

  if (jwe != NULL && json_is_object(j_header)) {
    if (json_object_get(j_header, "alg") != NULL) {
      if ((alg = r_str_to_jwa_alg(json_string_value(json_object_get(j_header, "alg")))) != R_JWA_ALG_UNKNOWN) {
        jwe->alg = alg;
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_set_full_header_json_t - Error invalid alg parameter");
        ret = RHN_ERROR_PARAM;
      }
    }
    if (json_object_get(j_header, "enc") != NULL) {
      if ((enc = r_str_to_jwa_enc(json_string_value(json_object_get(j_header, "enc")))) != R_JWA_ENC_UNKNOWN) {
        jwe->enc = enc;
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_set_full_header_json_t - Error invalid enc parameter");
        ret = RHN_ERROR_PARAM;
      }
    }
    if (ret == RHN_OK) {
      json_decref(jwe->j_header);
      if ((jwe->j_header = json_deep_copy(j_header)) == NULL) {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_set_full_header_json_t - Error setting header");
        ret = RHN_ERROR_MEMORY;
      }
    }
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_set_full_header_json_t - Error input parameters");
    ret = RHN_ERROR_PARAM;
  }
  return ret;
}