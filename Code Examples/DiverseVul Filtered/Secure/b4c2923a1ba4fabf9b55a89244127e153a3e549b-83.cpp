jwe_t * r_jwe_quick_parsen(const char * jwe_str, size_t jwe_str_len, uint32_t parse_flags, int x5u_flags) {
  jwe_t * jwe = NULL;
  int ret;

  if (r_jwe_init(&jwe) == RHN_OK) {
    ret = r_jwe_advanced_parsen(jwe, jwe_str, jwe_str_len, parse_flags, x5u_flags);
    if (ret != RHN_OK) {
      r_jwe_free(jwe);
      jwe = NULL;
    }
  } else {
    r_jwe_free(jwe);
    jwe = NULL;
  }
  return jwe;
}