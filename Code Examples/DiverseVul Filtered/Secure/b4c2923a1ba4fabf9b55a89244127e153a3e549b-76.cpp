static gnutls_cipher_algorithm_t r_jwe_get_alg_from_alg(jwa_alg alg) {
  gnutls_cipher_algorithm_t ret_alg = GNUTLS_CIPHER_UNKNOWN;

  switch (alg) {
    case R_JWA_ALG_A128GCMKW:
      ret_alg = GNUTLS_CIPHER_AES_128_GCM;
      break;
    case R_JWA_ALG_A192GCMKW:
#if GNUTLS_VERSION_NUMBER >= 0x03060e
      ret_alg = GNUTLS_CIPHER_AES_192_GCM;
#else
      ret_alg = GNUTLS_CIPHER_UNKNOWN; // Unsupported until GnuTLS 3.6.14
#endif
      break;
    case R_JWA_ALG_A256GCMKW:
      ret_alg = GNUTLS_CIPHER_AES_256_GCM;
      break;
    default:
      ret_alg = GNUTLS_CIPHER_UNKNOWN;
      break;
  }
  return ret_alg;
}