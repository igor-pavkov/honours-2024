int r_jwe_set_header_str_value(jwe_t * jwe, const char * key, const char * str_value) {
  int ret;

  if (jwe != NULL) {
    if ((ret = _r_json_set_str_value(jwe->j_header, key, str_value)) == RHN_OK) {
      o_free(jwe->header_b64url);
      jwe->header_b64url = NULL;
    }
    return ret;
  } else {
    return RHN_ERROR_PARAM;
  }
}