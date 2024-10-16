int r_jwe_advanced_compact_parsen(jwe_t * jwe, const char * jwe_str, size_t jwe_str_len, uint32_t parse_flags, int x5u_flags) {
  int ret;
  char ** str_array = NULL;
  char * str_header = NULL, * token = NULL, * tmp;
  unsigned char * iv = NULL;
  size_t header_len = 0, cypher_key_len = 0, iv_len = 0, cypher_len = 0, tag_len = 0;
  json_t * j_header = NULL;

  if (jwe != NULL && jwe_str != NULL && jwe_str_len) {
    token = o_strndup(jwe_str, jwe_str_len);
    // Remove whitespaces and newlines
    tmp = str_replace(token, " ", "");
    o_free(token);
    token = tmp;
    tmp = str_replace(token, "\n", "");
    o_free(token);
    token = tmp;
    tmp = str_replace(token, "\t", "");
    o_free(token);
    token = tmp;
    tmp = str_replace(token, "\v", "");
    o_free(token);
    token = tmp;
    tmp = str_replace(token, "\f", "");
    o_free(token);
    token = tmp;
    tmp = str_replace(token, "\r", "");
    o_free(token);
    token = tmp;
    if (split_string(token, ".", &str_array) == 5 && !o_strnullempty(str_array[0]) && !o_strnullempty(str_array[2]) && !o_strnullempty(str_array[3]) && !o_strnullempty(str_array[4])) {
      // Check if all elements 0, 2 and 3 are base64url encoded
      if (o_base64url_decode((unsigned char *)str_array[0], o_strlen(str_array[0]), NULL, &header_len) &&
         (o_strnullempty(str_array[1]) || o_base64url_decode((unsigned char *)str_array[1], o_strlen(str_array[1]), NULL, &cypher_key_len)) &&
          o_base64url_decode((unsigned char *)str_array[2], o_strlen(str_array[2]), NULL, &iv_len) &&
          o_base64url_decode((unsigned char *)str_array[3], o_strlen(str_array[3]), NULL, &cypher_len) &&
          o_base64url_decode((unsigned char *)str_array[4], o_strlen(str_array[4]), NULL, &tag_len)) {
        ret = RHN_OK;
        jwe->token_mode = R_JSON_MODE_COMPACT;
        do {
          // Decode header
          if ((str_header = o_malloc(header_len+4)) == NULL) {
            y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_compact_parsen - Error allocating resources for str_header");
            ret = RHN_ERROR_MEMORY;
            break;
          }

          if (!o_base64url_decode((unsigned char *)str_array[0], o_strlen(str_array[0]), (unsigned char *)str_header, &header_len)) {
            y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_compact_parsen - Error o_base64url_decode str_header");
            ret = RHN_ERROR_PARAM;
            break;
          }
          str_header[header_len] = '\0';

          if ((j_header = json_loads(str_header, JSON_DECODE_ANY, NULL)) == NULL) {
            y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_compact_parsen - Error json_loads str_header");
            ret = RHN_ERROR_PARAM;
            break;
          }

          if (r_jwe_extract_header(jwe, j_header, parse_flags, x5u_flags) != RHN_OK) {
            y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_compact_parsen - error extracting header params");
            ret = RHN_ERROR_PARAM;
            break;
          }
          json_decref(jwe->j_header);

          jwe->j_header = json_incref(j_header);

          // Decode iv
          if ((iv = o_malloc(iv_len+4)) == NULL) {
            y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_compact_parsen - Error allocating resources for iv");
            ret = RHN_ERROR_MEMORY;
            break;
          }

          if (!o_base64url_decode((unsigned char *)str_array[2], o_strlen(str_array[2]), iv, &iv_len)) {
            y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_compact_parsen - Error o_base64url_decode iv");
            ret = RHN_ERROR_PARAM;
            break;
          }

          if (r_jwe_set_iv(jwe, iv, iv_len) != RHN_OK) {
            y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_compact_parsen - Error r_jwe_set_iv");
            ret = RHN_ERROR;
            break;
          }

          o_free(jwe->header_b64url);
          jwe->header_b64url = (unsigned char *)o_strdup(str_array[0]);
          o_free(jwe->aad_b64url);
          jwe->aad_b64url = (unsigned char *)o_strdup(str_array[0]);
          o_free(jwe->encrypted_key_b64url);
          jwe->encrypted_key_b64url = (unsigned char *)o_strdup(str_array[1]);
          o_free(jwe->iv_b64url);
          jwe->iv_b64url = (unsigned char *)o_strdup(str_array[2]);
          o_free(jwe->ciphertext_b64url);
          jwe->ciphertext_b64url = (unsigned char *)o_strdup(str_array[3]);
          o_free(jwe->auth_tag_b64url);
          jwe->auth_tag_b64url = (unsigned char *)o_strdup(str_array[4]);

        } while (0);
        json_decref(j_header);
        o_free(str_header);
        o_free(iv);
      } else {
        ret = RHN_ERROR_PARAM;
      }
    } else {
      ret = RHN_ERROR_PARAM;
    }
    free_string_array(str_array);
    o_free(token);
  } else {
    ret = RHN_ERROR_PARAM;
  }
  return ret;
}