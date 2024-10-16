int r_jwe_set_header_json_t_value(jwe_t * jwe, const char * key, json_t * j_value) {
  int ret;

  if (jwe != NULL) {
    if ((ret = _r_json_set_json_t_value(jwe->j_header, key, j_value)) == RHN_OK) {
      o_free(jwe->header_b64url);
      jwe->header_b64url = NULL;
    }
    return ret;
  } else {
    ret = RHN_ERROR_PARAM;
  }
  return ret;
}