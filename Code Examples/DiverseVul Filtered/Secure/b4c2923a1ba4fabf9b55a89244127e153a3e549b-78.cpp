static int _r_rsa_oaep_encrypt(gnutls_pubkey_t g_pub, jwa_alg alg, uint8_t * cleartext, size_t cleartext_len, uint8_t * ciphertext, size_t * cyphertext_len) {
  struct rsa_public_key pub;
  gnutls_datum_t m = {NULL, 0}, e = {NULL, 0};
  int ret = RHN_OK;
  mpz_t gibberish;

  rsa_public_key_init(&pub);
  mpz_init(gibberish);
  if (gnutls_pubkey_export_rsa_raw(g_pub, &m, &e) == GNUTLS_E_SUCCESS) {
    mpz_import(pub.n, m.size, 1, 1, 0, 0, m.data);
    mpz_import(pub.e, e.size, 1, 1, 0, 0, e.data);
    rsa_public_key_prepare(&pub);
    if (*cyphertext_len >= pub.size) {
      if (alg == R_JWA_ALG_RSA_OAEP) {
        if (!rsa_oaep_sha1_encrypt(&pub, NULL, rnd_nonce_func, 0, NULL, cleartext_len, cleartext, gibberish)) {
          y_log_message(Y_LOG_LEVEL_ERROR, "_r_rsa_oaep_encrypt - Error rsa_oaep_sha1_encrypt");
          ret = RHN_ERROR;
        }
      } else {
        if (!rsa_oaep_sha256_encrypt(&pub, NULL, rnd_nonce_func, 0, NULL, cleartext_len, cleartext, gibberish)) {
          y_log_message(Y_LOG_LEVEL_ERROR, "_r_rsa_oaep_encrypt - Error rsa_oaep_sha256_encrypt");
          ret = RHN_ERROR;
        }
      }
      if (ret == RHN_OK) {
        nettle_mpz_get_str_256(pub.size, ciphertext, gibberish);
        *cyphertext_len = pub.size;
      }
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "_r_rsa_oaep_encrypt - Error cyphertext to small");
      ret = RHN_ERROR_PARAM;
    }
    gnutls_free(m.data);
    gnutls_free(e.data);
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "_r_rsa_oaep_encrypt - Error gnutls_pubkey_export_rsa_raw");
    ret = RHN_ERROR;
  }
  rsa_public_key_clear(&pub);
  mpz_clear(gibberish);

  return ret;
}