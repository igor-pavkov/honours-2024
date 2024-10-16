static int _r_dh_compute(uint8_t * priv_k, uint8_t * pub_x, size_t crv_size, gnutls_datum_t * Z) {
  int ret;
  uint8_t q[CURVE448_SIZE] = {0};

  if (crv_size == CURVE25519_SIZE) {
    curve25519_mul(q, priv_k, pub_x);
  } else {
    curve448_mul(q, priv_k, pub_x);
  }

  if ((Z->data = gnutls_malloc(crv_size)) != NULL) {
    memcpy(Z->data, q, crv_size);
    Z->size = crv_size;
    ret = RHN_OK;
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "_r_dh_compute - Error gnutls_malloc");
    ret = RHN_ERROR_MEMORY;
  }

  return ret;
}