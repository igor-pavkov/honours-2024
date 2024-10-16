int r_jwe_set_header_int_value(jwe_t * jwe, const char * key, rhn_int_t i_value) {
  int ret;

  if (jwe != NULL) {
    if ((ret = _r_json_set_int_value(jwe->j_header, key, i_value)) == RHN_OK) {
      o_free(jwe->header_b64url);
      jwe->header_b64url = NULL;
    }
    return ret;
  } else {
    return RHN_ERROR_PARAM;
  }
}