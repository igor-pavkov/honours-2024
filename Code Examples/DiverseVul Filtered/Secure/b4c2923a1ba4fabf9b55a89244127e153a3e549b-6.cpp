static int _r_rsa_oaep_decrypt(gnutls_privkey_t g_priv, jwa_alg alg, uint8_t * ciphertext, size_t cyphertext_len, uint8_t * cleartext, size_t * cleartext_len) {
  struct rsa_private_key priv;
  gnutls_datum_t m = {NULL, 0}, e = {NULL, 0}, d = {NULL, 0}, p = {NULL, 0}, q = {NULL, 0}, u = {NULL, 0}, e1 = {NULL, 0}, e2 = {NULL, 0};
  int ret = RHN_OK;
  mpz_t gibberish;

  rsa_private_key_init(&priv);
  mpz_init(gibberish);
  nettle_mpz_set_str_256_u(gibberish, cyphertext_len, ciphertext);
  if (gnutls_privkey_export_rsa_raw(g_priv, &m, &e, &d, &p, &q, &u, &e1, &e2) == GNUTLS_E_SUCCESS) {
    mpz_import(priv.d, d.size, 1, 1, 0, 0, d.data);
    mpz_import(priv.p, p.size, 1, 1, 0, 0, p.data);
    mpz_import(priv.q, q.size, 1, 1, 0, 0, q.data);
    mpz_import(priv.a, e1.size, 1, 1, 0, 0, e1.data);
    mpz_import(priv.b, e2.size, 1, 1, 0, 0, e2.data);
    mpz_import(priv.c, u.size, 1, 1, 0, 0, u.data);
    rsa_private_key_prepare(&priv);
    if (cyphertext_len >= priv.size) {
      if (alg == R_JWA_ALG_RSA_OAEP) {
        if (!rsa_oaep_sha1_decrypt(&priv, 0, NULL, cleartext_len, cleartext, gibberish)) {
          y_log_message(Y_LOG_LEVEL_ERROR, "_r_rsa_oaep_decrypt - Error rsa_oaep_sha1_decrypt");
          ret = RHN_ERROR;
        }
      } else {
        if (!rsa_oaep_sha256_decrypt(&priv, 0, NULL, cleartext_len, cleartext, gibberish)) {
          y_log_message(Y_LOG_LEVEL_ERROR, "_r_rsa_oaep_decrypt - Error rsa_oaep_sha256_decrypt");
          ret = RHN_ERROR;
        }
      }
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "_r_rsa_oaep_encrypt - Error cyphertext to small");
      ret = RHN_ERROR_PARAM;
    }
    gnutls_free(m.data);
    gnutls_free(e.data);
    gnutls_free(d.data);
    gnutls_free(p.data);
    gnutls_free(q.data);
    gnutls_free(u.data);
    gnutls_free(e1.data);
    gnutls_free(e2.data);
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "_r_rsa_oaep_encrypt - Error gnutls_pubkey_export_rsa_raw");
    ret = RHN_ERROR;
  }
  rsa_private_key_clear(&priv);
  mpz_clear(gibberish);

  return ret;
}