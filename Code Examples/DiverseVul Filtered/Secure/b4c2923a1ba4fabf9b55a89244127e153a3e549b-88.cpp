int r_jwe_compact_parsen(jwe_t * jwe, const char * jwe_str, size_t jwe_str_len, int x5u_flags) {
  return r_jwe_advanced_compact_parsen(jwe, jwe_str, jwe_str_len, R_PARSE_HEADER_ALL, x5u_flags);
}