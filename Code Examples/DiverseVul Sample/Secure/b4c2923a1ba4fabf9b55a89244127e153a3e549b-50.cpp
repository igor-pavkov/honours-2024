jwe_t * r_jwe_copy(jwe_t * jwe) {
  jwe_t * jwe_copy = NULL;

  if (jwe != NULL) {
    if (r_jwe_init(&jwe_copy) == RHN_OK) {
      jwe_copy->alg = jwe->alg;
      jwe_copy->enc = jwe->enc;
      jwe_copy->token_mode = jwe->token_mode;
      if (r_jwe_set_payload(jwe_copy, jwe->payload, jwe->payload_len) == RHN_OK &&
          r_jwe_set_iv(jwe_copy, jwe->iv, jwe->iv_len) == RHN_OK &&
          r_jwe_set_aad(jwe_copy, jwe->aad, jwe->aad_len) == RHN_OK &&
          r_jwe_set_cypher_key(jwe_copy, jwe->key, jwe->key_len) == RHN_OK &&
          r_jwe_set_alg(jwe_copy, r_jwe_get_alg(jwe)) == RHN_OK) {
        jwe_copy->header_b64url = (unsigned char *)o_strdup((const char *)jwe->header_b64url);
        jwe_copy->encrypted_key_b64url = (unsigned char *)o_strdup((const char *)jwe->encrypted_key_b64url);
        jwe_copy->ciphertext_b64url = (unsigned char *)o_strdup((const char *)jwe->ciphertext_b64url);
        jwe_copy->auth_tag_b64url = (unsigned char *)o_strdup((const char *)jwe->auth_tag_b64url);
        r_jwks_free(jwe_copy->jwks_privkey);
        jwe_copy->jwks_privkey = r_jwks_copy(jwe->jwks_privkey);
        r_jwks_free(jwe_copy->jwks_pubkey);
        jwe_copy->jwks_pubkey = r_jwks_copy(jwe->jwks_pubkey);
        json_decref(jwe_copy->j_header);
        jwe_copy->j_header = json_deep_copy(jwe->j_header);
        jwe_copy->j_unprotected_header = json_deep_copy(jwe->j_unprotected_header);
        jwe_copy->j_json_serialization = json_deep_copy(jwe->j_json_serialization);
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jwe_copy - Error setting values");
        r_jwe_free(jwe_copy);
        jwe_copy = NULL;
      }
    }
  }
  return jwe_copy;
}