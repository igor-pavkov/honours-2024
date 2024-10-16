int r_jwe_parsen_json_str(jwe_t * jwe, const char * jwe_json_str, size_t jwe_json_str_len, int x5u_flags) {
  json_t * jwe_json = NULL;
  int ret;

  jwe_json = json_loadb(jwe_json_str, jwe_json_str_len, JSON_DECODE_ANY, NULL);
  ret = r_jwe_parse_json_t(jwe, jwe_json, x5u_flags);
  json_decref(jwe_json);

  return ret;
}