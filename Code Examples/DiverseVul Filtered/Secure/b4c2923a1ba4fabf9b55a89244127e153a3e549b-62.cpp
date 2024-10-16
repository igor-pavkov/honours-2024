void r_jwe_free(jwe_t * jwe) {
  if (jwe != NULL) {
    r_jwks_free(jwe->jwks_privkey);
    r_jwks_free(jwe->jwks_pubkey);
    o_free(jwe->header_b64url);
    o_free(jwe->encrypted_key_b64url);
    o_free(jwe->iv_b64url);
    o_free(jwe->aad_b64url);
    o_free(jwe->ciphertext_b64url);
    o_free(jwe->auth_tag_b64url);
    json_decref(jwe->j_header);
    json_decref(jwe->j_unprotected_header);
    json_decref(jwe->j_json_serialization);
    o_free(jwe->key);
    o_free(jwe->iv);
    o_free(jwe->aad);
    o_free(jwe->payload);
    o_free(jwe);
  }
}