_gnutls_x509_pk_to_sign (gnutls_pk_algorithm_t pk, gnutls_mac_algorithm_t mac)
{
  gnutls_sign_algorithm_t ret = 0;

  GNUTLS_SIGN_LOOP (if (pk == p->pk && mac == p->mac)
		    {
		    ret = p->id; break;}
  );

  if (ret == 0)
    return GNUTLS_SIGN_UNKNOWN;
  return ret;
}