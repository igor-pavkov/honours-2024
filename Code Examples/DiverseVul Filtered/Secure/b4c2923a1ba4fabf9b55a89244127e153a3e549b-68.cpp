static gnutls_mac_algorithm_t r_jwe_get_digest_from_enc(jwa_enc enc) {
  gnutls_mac_algorithm_t digest;

  switch (enc) {
    case R_JWA_ENC_A128CBC:
      digest = GNUTLS_MAC_SHA256;
      break;
    case R_JWA_ENC_A192CBC:
      digest = GNUTLS_MAC_SHA384;
      break;
    case R_JWA_ENC_A256CBC:
      digest = GNUTLS_MAC_SHA512;
      break;
    case R_JWA_ENC_A128GCM:
      digest = GNUTLS_MAC_SHA256;
      break;
    case R_JWA_ENC_A192GCM:
      digest = GNUTLS_MAC_SHA384;
      break;
    case R_JWA_ENC_A256GCM:
      digest = GNUTLS_MAC_SHA512;
      break;
    default:
      digest = GNUTLS_MAC_UNKNOWN;
      break;
  }
  return digest;
}