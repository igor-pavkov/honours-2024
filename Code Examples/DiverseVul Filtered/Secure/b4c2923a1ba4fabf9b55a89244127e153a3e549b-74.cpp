const char * r_jwe_get_kid(jwe_t * jwe) {
  return r_jwe_get_header_str_value(jwe, "kid");
}