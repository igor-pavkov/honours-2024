int r_jwe_advanced_parse_json_t(jwe_t * jwe, json_t * jwe_json, uint32_t parse_flags, int x5u_flags) {
  int ret;
  size_t header_len = 0, cypher_key_len = 0, iv_len = 0, index = 0;;
  char * str_header = NULL;
  json_t * j_header = NULL, * j_recipient;
  unsigned char * iv = NULL;

  if (jwe != NULL && json_is_object(jwe_json)) {
    if (json_string_length(json_object_get(jwe_json, "protected")) &&
        json_string_length(json_object_get(jwe_json, "iv")) &&
        json_string_length(json_object_get(jwe_json, "ciphertext")) &&
        json_string_length(json_object_get(jwe_json, "tag"))) {
      ret = RHN_OK;
      r_jwe_set_cypher_key(jwe, NULL, 0);
      r_jwe_set_iv(jwe, NULL, 0);
      r_jwe_set_aad(jwe, NULL, 0);
      r_jwe_set_payload(jwe, NULL, 0);
      o_free(jwe->header_b64url);
      jwe->header_b64url = NULL;
      o_free(jwe->encrypted_key_b64url);
      jwe->encrypted_key_b64url = NULL;
      o_free(jwe->iv_b64url);
      jwe->iv_b64url = NULL;
      o_free(jwe->ciphertext_b64url);
      jwe->ciphertext_b64url = NULL;
      o_free(jwe->auth_tag_b64url);
      jwe->auth_tag_b64url = NULL;
      o_free(jwe->aad_b64url);
      jwe->aad_b64url = NULL;
      json_decref(jwe->j_header);
      jwe->j_header = json_object();
      json_decref(jwe->j_unprotected_header);
      jwe->j_unprotected_header = NULL;
      do {
        json_decref(jwe->j_json_serialization);
        if ((jwe->j_json_serialization = json_deep_copy(jwe_json)) == NULL) {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_parse_json_t - Error setting j_json_serialization");
          ret = RHN_ERROR;
          break;
        }

        if (json_object_get(jwe_json, "unprotected") != NULL && r_jwe_set_full_unprotected_header_json_t(jwe, json_object_get(jwe_json, "unprotected")) != RHN_OK) {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_parse_json_t - Error r_jwe_set_full_unprotected_header_json_t");
          ret = RHN_ERROR_PARAM;
          break;
        }

        if (!o_base64url_decode((unsigned char *)json_string_value(json_object_get(jwe_json, "protected")), json_string_length(json_object_get(jwe_json, "protected")), NULL, &header_len)) {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_parse_json_t - Error invalid protected base64");
          ret = RHN_ERROR_PARAM;
          break;
        }

        if ((str_header = o_malloc(header_len+4)) == NULL) {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_parse_json_t - Error allocating resources for str_header");
          ret = RHN_ERROR_PARAM;
          break;
        }

        if (!o_base64url_decode((unsigned char *)json_string_value(json_object_get(jwe_json, "protected")), json_string_length(json_object_get(jwe_json, "protected")), (unsigned char *)str_header, &header_len)) {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_parse_json_t - Error invalid protected base64");
          ret = RHN_ERROR_PARAM;
          break;
        }
        str_header[header_len] = '\0';

        if ((j_header = json_loads(str_header, JSON_DECODE_ANY, NULL)) == NULL) {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_parse_json_t - Error json_loads str_header");
          ret = RHN_ERROR_PARAM;
          break;
        }

        if (r_jwe_extract_header(jwe, j_header, parse_flags, x5u_flags) != RHN_OK) {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_parse_json_t - error extracting header params");
          ret = RHN_ERROR_PARAM;
          break;
        }
        json_decref(jwe->j_header);

        jwe->j_header = json_incref(j_header);

        // Decode iv
        if (!o_base64url_decode((unsigned char *)json_string_value(json_object_get(jwe_json, "iv")), json_string_length(json_object_get(jwe_json, "iv")), NULL, &iv_len)) {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_parse_json_t - Error invalid iv base64");
          ret = RHN_ERROR_PARAM;
          break;
        }

        if ((iv = o_malloc(iv_len+4)) == NULL) {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_parse_json_t - Error allocating resources for iv");
          ret = RHN_ERROR_MEMORY;
          break;
        }

        if (!o_base64url_decode((unsigned char *)json_string_value(json_object_get(jwe_json, "iv")), json_string_length(json_object_get(jwe_json, "iv")), iv, &iv_len)) {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_parse_json_t - Error o_base64url_decode iv");
          ret = RHN_ERROR_PARAM;
          break;
        }

        if (r_jwe_set_iv(jwe, iv, iv_len) != RHN_OK) {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_parse_json_t - Error r_jwe_set_iv");
          ret = RHN_ERROR;
          break;
        }
        jwe->header_b64url = (unsigned char *)o_strdup(json_string_value(json_object_get(jwe_json, "protected")));
        jwe->ciphertext_b64url = (unsigned char *)o_strdup(json_string_value(json_object_get(jwe_json, "ciphertext")));
        jwe->auth_tag_b64url = (unsigned char *)o_strdup(json_string_value(json_object_get(jwe_json, "tag")));
        jwe->aad_b64url = (unsigned char *)o_strdup(json_string_value(json_object_get(jwe_json, "aad")));

      } while (0);
      json_decref(j_header);
      o_free(str_header);
      o_free(iv);
      if (ret == RHN_OK) {
        if (json_array_size(json_object_get(jwe_json, "recipients"))) {
          jwe->token_mode = R_JSON_MODE_GENERAL;
          json_array_foreach(json_object_get(jwe_json, "recipients"), index, j_recipient) {
            if (!json_is_object(j_recipient)) {
              y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_parse_json_t - Invalid recipient at index %zu, must be a JSON object", index);
              ret = RHN_ERROR_PARAM;
              break;
            } else {
              if (!o_base64url_decode((const unsigned char*)json_string_value(json_object_get(j_recipient, "encrypted_key")), json_string_length(json_object_get(j_recipient, "encrypted_key")), NULL, &cypher_key_len)) {
                y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_parse_json_t - Error at index %zu, invalid encrypted_key base64 %s", index);
                ret = RHN_ERROR_PARAM;
                break;
              }
              if (json_object_get(j_recipient, "header") != NULL && !json_is_object(json_object_get(j_recipient, "header"))) {
                y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_parse_json_t - Invalid header at index %zu, must be a JSON object", index);
                ret = RHN_ERROR_PARAM;
                break;
              }
            }
          }
        } else {
          jwe->token_mode = R_JSON_MODE_FLATTENED;
          jwe->encrypted_key_b64url = (unsigned char *)o_strdup(json_string_value(json_object_get(jwe_json, "encrypted_key")));
          if (json_object_get(jwe_json, "header") == NULL || r_jwe_extract_header(jwe, json_object_get(jwe_json, "header"), parse_flags, x5u_flags) == RHN_OK) {
            json_object_update_missing(jwe->j_header, json_object_get(jwe_json, "header"));
          } else {
            y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_parse_json_t - error extracting header params");
            ret = RHN_ERROR_PARAM;
          }
        }
      }
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_parse_json_t - Error invalid content");
      ret = RHN_ERROR_PARAM;
    }
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_parse_json_t - Error input parameters");
    ret = RHN_ERROR_PARAM;
  }
  return ret;
}