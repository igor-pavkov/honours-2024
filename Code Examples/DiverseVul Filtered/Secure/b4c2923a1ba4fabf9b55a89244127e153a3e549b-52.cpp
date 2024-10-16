int r_jwe_compact_parse(jwe_t * jwe, const char * jwe_str, int x5u_flags) {
  return r_jwe_compact_parsen(jwe, jwe_str, o_strlen(jwe_str), x5u_flags);
}