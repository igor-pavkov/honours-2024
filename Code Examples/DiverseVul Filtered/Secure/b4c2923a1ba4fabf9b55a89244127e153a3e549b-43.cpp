int r_jwe_parsen(jwe_t * jwe, const char * jwe_str, size_t jwe_str_len, int x5u_flags) {
  int ret;
  char * str = (char *)jwe_str;

  if (jwe != NULL && str != NULL && jwe_str_len) {
    while(isspace((unsigned char)*str) && jwe_str_len) {
      str++;
      jwe_str_len--;
    }

    if (0 == o_strncmp("ey", str, 2)) {
      ret = r_jwe_compact_parsen(jwe, jwe_str, jwe_str_len, x5u_flags);
    } else if (*str == '{') {
      ret = r_jwe_parsen_json_str(jwe, jwe_str, jwe_str_len, x5u_flags);
    } else {
      ret = RHN_ERROR_PARAM;
    }
  } else {
    ret = RHN_ERROR_PARAM;
  }
  return ret;
}