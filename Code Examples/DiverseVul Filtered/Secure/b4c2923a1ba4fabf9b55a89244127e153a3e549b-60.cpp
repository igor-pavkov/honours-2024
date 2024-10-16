char * r_jwe_serialize_json_str(jwe_t * jwe, jwks_t * jwks_pubkey, int x5u_flags, int mode) {
  json_t * j_result = r_jwe_serialize_json_t(jwe, jwks_pubkey, x5u_flags, mode);
  char * str_result = json_dumps(j_result, JSON_COMPACT);
  json_decref(j_result);
  return str_result;
}