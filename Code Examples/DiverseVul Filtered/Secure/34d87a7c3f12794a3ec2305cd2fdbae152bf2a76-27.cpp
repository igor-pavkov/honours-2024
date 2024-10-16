gnutls_cipher_suite_get_name (gnutls_kx_algorithm_t
			      kx_algorithm,
			      gnutls_cipher_algorithm_t
			      cipher_algorithm,
			      gnutls_mac_algorithm_t mac_algorithm)
{
  const char *ret = NULL;

  /* avoid prefix */
  GNUTLS_CIPHER_SUITE_LOOP (if (kx_algorithm == p->kx_algorithm &&
				cipher_algorithm == p->block_algorithm &&
				mac_algorithm == p->mac_algorithm)
			    ret = p->name + sizeof ("GNUTLS_") - 1);

  return ret;
}