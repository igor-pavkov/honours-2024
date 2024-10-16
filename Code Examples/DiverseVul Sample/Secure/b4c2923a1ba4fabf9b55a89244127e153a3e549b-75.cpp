char * r_jwe_get_full_header_str(jwe_t * jwe) {
  char * to_return = NULL;
  if (jwe != NULL) {
    to_return = json_dumps(jwe->j_header, JSON_COMPACT);
  }
  return to_return;
}