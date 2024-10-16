int r_jwe_set_properties(jwe_t * jwe, ...) {
  rhn_opt option;
  int ret = RHN_OK;
  int i_value;
  rhn_int_t r_value;
  unsigned int ui_value;
  const char * str_key, * str_value;
  json_t * j_value;
  const unsigned char * ustr_value;
  size_t size_value;
  jwk_t * jwk;
  jwks_t * jwks;
  gnutls_privkey_t privkey;
  gnutls_pubkey_t pubkey;
  va_list vl;

  if (jwe != NULL) {
    va_start(vl, jwe);
    for (option = va_arg(vl, rhn_opt); option != RHN_OPT_NONE && ret == RHN_OK; option = va_arg(vl, rhn_opt)) {
      switch (option) {
        case RHN_OPT_HEADER_INT_VALUE:
          str_key = va_arg(vl, const char *);
          i_value = va_arg(vl, int);
          ret = r_jwe_set_header_int_value(jwe, str_key, (rhn_int_t)i_value);
          break;
        case RHN_OPT_HEADER_RHN_INT_VALUE:
          str_key = va_arg(vl, const char *);
          r_value = va_arg(vl, rhn_int_t);
          ret = r_jwe_set_header_int_value(jwe, str_key, r_value);
          break;
        case RHN_OPT_HEADER_STR_VALUE:
          str_key = va_arg(vl, const char *);
          str_value = va_arg(vl, const char *);
          ret = r_jwe_set_header_str_value(jwe, str_key, str_value);
          break;
        case RHN_OPT_HEADER_JSON_T_VALUE:
          str_key = va_arg(vl, const char *);
          j_value = va_arg(vl, json_t *);
          ret = r_jwe_set_header_json_t_value(jwe, str_key, j_value);
          break;
        case RHN_OPT_HEADER_FULL_JSON_T:
          j_value = va_arg(vl, json_t *);
          ret = r_jwe_set_full_header_json_t(jwe, j_value);
          break;
        case RHN_OPT_HEADER_FULL_JSON_STR:
          str_value = va_arg(vl, const char *);
          ret = r_jwe_set_full_header_json_str(jwe, str_value);
          break;
        case RHN_OPT_UN_HEADER_FULL_JSON_T:
          j_value = va_arg(vl, json_t *);
          ret = r_jwe_set_full_unprotected_header_json_t(jwe, j_value);
          break;
        case RHN_OPT_UN_HEADER_FULL_JSON_STR:
          str_value = va_arg(vl, const char *);
          ret = r_jwe_set_full_unprotected_header_json_str(jwe, str_value);
          break;
        case RHN_OPT_PAYLOAD:
          ustr_value = va_arg(vl, const unsigned char *);
          size_value = va_arg(vl, size_t);
          ret = r_jwe_set_payload(jwe, ustr_value, size_value);
          break;
        case RHN_OPT_ENC_ALG:
          ui_value = va_arg(vl, unsigned int);
          ret = r_jwe_set_alg(jwe, (jwa_alg)ui_value);
          break;
        case RHN_OPT_ENC:
          ui_value = va_arg(vl, unsigned int);
          ret = r_jwe_set_enc(jwe, (jwa_enc)ui_value);
          break;
        case RHN_OPT_CIPHER_KEY:
          ustr_value = va_arg(vl, const unsigned char *);
          size_value = va_arg(vl, size_t);
          ret = r_jwe_set_cypher_key(jwe, ustr_value, size_value);
          break;
        case RHN_OPT_IV:
          ustr_value = va_arg(vl, const unsigned char *);
          size_value = va_arg(vl, size_t);
          ret = r_jwe_set_iv(jwe, ustr_value, size_value);
          break;
        case RHN_OPT_AAD:
          ustr_value = va_arg(vl, const unsigned char *);
          size_value = va_arg(vl, size_t);
          ret = r_jwe_set_aad(jwe, ustr_value, size_value);
          break;
        case RHN_OPT_ENCRYPT_KEY_JWK:
          jwk = va_arg(vl, jwk_t *);
          ret = r_jwe_add_keys(jwe, NULL, jwk);
          break;
        case RHN_OPT_ENCRYPT_KEY_JWKS:
          jwks = va_arg(vl, jwks_t *);
          ret = r_jwe_add_jwks(jwe, NULL, jwks);
          break;
        case RHN_OPT_ENCRYPT_KEY_GNUTLS:
          pubkey = va_arg(vl, gnutls_pubkey_t);
          ret = r_jwe_add_keys_gnutls(jwe, NULL, pubkey);
          break;
        case RHN_OPT_ENCRYPT_KEY_JSON_T:
          j_value = va_arg(vl, json_t *);
          ret = r_jwe_add_keys_json_t(jwe, NULL, j_value);
          break;
        case RHN_OPT_ENCRYPT_KEY_JSON_STR:
          str_value = va_arg(vl, const char *);
          ret = r_jwe_add_keys_json_str(jwe, NULL, str_value);
          break;
        case RHN_OPT_ENCRYPT_KEY_PEM_DER:
          ui_value = va_arg(vl, unsigned int);
          ustr_value = va_arg(vl, const unsigned char *);
          size_value = va_arg(vl, size_t);
          ret = r_jwe_add_keys_pem_der(jwe, ui_value, NULL, 0, ustr_value, size_value);
          break;
        case RHN_OPT_DECRYPT_KEY_JWK:
          jwk = va_arg(vl, jwk_t *);
          ret = r_jwe_add_keys(jwe, jwk, NULL);
          break;
        case RHN_OPT_DECRYPT_KEY_JWKS:
          jwks = va_arg(vl, jwks_t *);
          ret = r_jwe_add_jwks(jwe, jwks, NULL);
          break;
        case RHN_OPT_DECRYPT_KEY_GNUTLS:
          privkey = va_arg(vl, gnutls_privkey_t);
          ret = r_jwe_add_keys_gnutls(jwe, privkey, NULL);
          break;
        case RHN_OPT_DECRYPT_KEY_JSON_T:
          j_value = va_arg(vl, json_t *);
          ret = r_jwe_add_keys_json_t(jwe, j_value, NULL);
          break;
        case RHN_OPT_DECRYPT_KEY_JSON_STR:
          str_value = va_arg(vl, const char *);
          ret = r_jwe_add_keys_json_str(jwe, str_value, NULL);
          break;
        case RHN_OPT_DECRYPT_KEY_PEM_DER:
          ui_value = va_arg(vl, unsigned int);
          ustr_value = va_arg(vl, const unsigned char *);
          size_value = va_arg(vl, size_t);
          ret = r_jwe_add_keys_pem_der(jwe, ui_value, ustr_value, size_value, NULL, 0);
          break;
        default:
          ret = RHN_ERROR_PARAM;
          break;
      }
    }
    va_end(vl);
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_set_properties - Error input parameter");
    ret = RHN_ERROR_PARAM;
  }
  return ret;
}