int r_jwe_set_full_unprotected_header_json_t(jwe_t * jwe, json_t * j_unprotected_header) {
  int ret = RHN_OK;

  if (jwe != NULL && json_is_object(j_unprotected_header)) {
    json_decref(jwe->j_unprotected_header);
    if ((jwe->j_unprotected_header = json_deep_copy(j_unprotected_header)) == NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_set_full_unprotected_header_json_t - Error setting header");
      ret = RHN_ERROR_MEMORY;
    }
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_set_full_unprotected_header_json_t - Error input parameters");
    ret = RHN_ERROR_PARAM;
  }
  return ret;
}