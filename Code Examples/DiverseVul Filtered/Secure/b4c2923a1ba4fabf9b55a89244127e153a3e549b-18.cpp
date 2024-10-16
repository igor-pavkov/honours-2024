static int r_jwe_extract_header(jwe_t * jwe, json_t * j_header, uint32_t parse_flags, int x5u_flags) {
  int ret;
  jwk_t * jwk;

  if (json_is_object(j_header)) {
    ret = RHN_OK;

    if (json_object_get(j_header, "alg") != NULL) {
      if (0 != o_strcmp("RSA1_5", json_string_value(json_object_get(j_header, "alg"))) &&
      0 != o_strcmp("RSA-OAEP", json_string_value(json_object_get(j_header, "alg"))) &&
      0 != o_strcmp("RSA-OAEP-256", json_string_value(json_object_get(j_header, "alg"))) &&
      0 != o_strcmp("A128KW", json_string_value(json_object_get(j_header, "alg"))) &&
      0 != o_strcmp("A192KW", json_string_value(json_object_get(j_header, "alg"))) &&
      0 != o_strcmp("A256KW", json_string_value(json_object_get(j_header, "alg"))) &&
      0 != o_strcmp("dir", json_string_value(json_object_get(j_header, "alg"))) &&
      0 != o_strcmp("ECDH-ES", json_string_value(json_object_get(j_header, "alg"))) &&
      0 != o_strcmp("ECDH-ES+A128KW", json_string_value(json_object_get(j_header, "alg"))) &&
      0 != o_strcmp("ECDH-ES+A192KW", json_string_value(json_object_get(j_header, "alg"))) &&
      0 != o_strcmp("ECDH-ES+A256KW", json_string_value(json_object_get(j_header, "alg"))) &&
      0 != o_strcmp("A128GCMKW", json_string_value(json_object_get(j_header, "alg"))) &&
      0 != o_strcmp("A192GCMKW", json_string_value(json_object_get(j_header, "alg"))) &&
      0 != o_strcmp("A256GCMKW", json_string_value(json_object_get(j_header, "alg"))) &&
      0 != o_strcmp("PBES2-HS256+A128KW", json_string_value(json_object_get(j_header, "alg"))) &&
      0 != o_strcmp("PBES2-HS384+A192KW", json_string_value(json_object_get(j_header, "alg"))) &&
      0 != o_strcmp("PBES2-HS512+A256KW", json_string_value(json_object_get(j_header, "alg")))) {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_extract_header - Invalid alg");
        ret = RHN_ERROR_PARAM;
      } else {
        jwe->alg = r_str_to_jwa_alg(json_string_value(json_object_get(j_header, "alg")));
      }
    }

    if (json_object_get(j_header, "enc") != NULL) {
      if (0 != o_strcmp("A128CBC-HS256", json_string_value(json_object_get(j_header, "enc"))) &&
      0 != o_strcmp("A192CBC-HS384", json_string_value(json_object_get(j_header, "enc"))) &&
      0 != o_strcmp("A256CBC-HS512", json_string_value(json_object_get(j_header, "enc"))) &&
      0 != o_strcmp("A128GCM", json_string_value(json_object_get(j_header, "enc"))) &&
      0 != o_strcmp("A192GCM", json_string_value(json_object_get(j_header, "enc"))) &&
      0 != o_strcmp("A256GCM", json_string_value(json_object_get(j_header, "enc")))) {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_extract_header - Invalid enc");
        ret = RHN_ERROR_PARAM;
      } else {
        jwe->enc = r_str_to_jwa_enc(json_string_value(json_object_get(j_header, "enc")));
      }
    }

    if (json_string_length(json_object_get(j_header, "jku")) && (parse_flags&R_PARSE_HEADER_JKU)) {
      if (r_jwks_import_from_uri(jwe->jwks_pubkey, json_string_value(json_object_get(j_header, "jku")), x5u_flags) != RHN_OK) {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_extract_header - Error loading jwks from uri %s", json_string_value(json_object_get(j_header, "jku")));
      }
    }

    if (json_object_get(j_header, "jwk") != NULL && (parse_flags&R_PARSE_HEADER_JWK)) {
      r_jwk_init(&jwk);
      if (r_jwk_import_from_json_t(jwk, json_object_get(j_header, "jwk")) == RHN_OK && r_jwk_key_type(jwk, NULL, 0)&R_KEY_TYPE_PUBLIC) {
        if (r_jwks_append_jwk(jwe->jwks_pubkey, jwk) != RHN_OK) {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_extract_header - Error parsing header jwk");
          ret = RHN_ERROR;
        }
      } else {
        ret = RHN_ERROR_PARAM;
      }
      r_jwk_free(jwk);
    }

    if (json_object_get(j_header, "x5u") != NULL && (parse_flags&R_PARSE_HEADER_X5U)) {
      r_jwk_init(&jwk);
      if (r_jwk_import_from_x5u(jwk, x5u_flags, json_string_value(json_object_get(j_header, "x5u"))) == RHN_OK) {
        if (r_jwks_append_jwk(jwe->jwks_pubkey, jwk) != RHN_OK) {
          ret = RHN_ERROR;
        }
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_extract_header - Error importing x5u");
        ret = RHN_ERROR_PARAM;
      }
      r_jwk_free(jwk);
    }

    if (json_object_get(j_header, "x5c") != NULL && (parse_flags&R_PARSE_HEADER_X5C)) {
      r_jwk_init(&jwk);
      if (r_jwk_import_from_x5c(jwk, json_string_value(json_array_get(json_object_get(j_header, "x5c"), 0))) == RHN_OK) {
        if (r_jwks_append_jwk(jwe->jwks_pubkey, jwk) != RHN_OK) {
          ret = RHN_ERROR;
        }
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_extract_header - Error importing x5c");
        ret = RHN_ERROR_PARAM;
      }
      r_jwk_free(jwk);
    }
  } else {
    ret = RHN_ERROR_PARAM;
  }

  return ret;
}