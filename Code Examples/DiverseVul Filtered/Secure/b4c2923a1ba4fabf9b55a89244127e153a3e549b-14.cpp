static int _r_ecdh_compute(uint8_t * priv_d, size_t pub_d_size, uint8_t * pub_x, size_t pub_x_size, uint8_t * pub_y, size_t pub_y_size, const struct ecc_curve * curve, gnutls_datum_t * Z) {
  int ret = RHN_OK;
  struct ecc_scalar priv;
  struct ecc_point pub, r;
  mpz_t z_priv_d, z_pub_x, z_pub_y, r_x, r_y;
  uint8_t r_x_u[64] = {0};
  size_t r_x_u_len = 64;

  mpz_init(z_priv_d);
  mpz_init(z_pub_x);
  mpz_init(z_pub_y);
  mpz_init(r_x);
  mpz_init(r_y);
  ecc_scalar_init(&priv, curve);
  ecc_point_init(&pub, curve);
  ecc_point_init(&r, curve);
  do {
    mpz_import(z_priv_d, pub_d_size, 1, 1, 0, 0, priv_d);
    if (!ecc_scalar_set(&priv, z_priv_d)) {
      y_log_message(Y_LOG_LEVEL_ERROR, "_r_ecdh_compute - Error ecc_scalar_set");
      ret = RHN_ERROR;
      break;
    }

    mpz_import(z_pub_x, pub_x_size, 1, 1, 0, 0, pub_x);
    mpz_import(z_pub_y, pub_y_size, 1, 1, 0, 0, pub_y);
    if (!ecc_point_set(&pub, z_pub_x, z_pub_y)) {
      y_log_message(Y_LOG_LEVEL_ERROR, "_r_ecdh_compute - Error ecc_point_set");
      ret = RHN_ERROR;
      break;
    }

    ecc_point_mul(&r, &priv, &pub);
    ecc_point_get(&r, r_x, r_y);

    mpz_export(r_x_u, &r_x_u_len, 1, 1, 0, 0, r_x);

    if ((Z->data = gnutls_malloc(r_x_u_len)) == NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "_r_ecdh_compute - Error gnutls_malloc");
      ret = RHN_ERROR_MEMORY;
      break;
    }
    memcpy(Z->data, r_x_u, r_x_u_len);
    Z->size = r_x_u_len;
    ret = RHN_OK;
  } while (0);
  mpz_clear(z_priv_d);
  mpz_clear(z_pub_x);
  mpz_clear(z_pub_y);
  mpz_clear(r_x);
  mpz_clear(r_y);
  ecc_scalar_clear(&priv);
  ecc_point_clear(&pub);
  ecc_point_clear(&r);

  return ret;
}