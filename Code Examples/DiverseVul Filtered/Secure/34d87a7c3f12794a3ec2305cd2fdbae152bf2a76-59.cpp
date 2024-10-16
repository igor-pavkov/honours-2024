_gnutls_kx_encipher_type (gnutls_kx_algorithm_t kx_algorithm)
{
  int ret = CIPHER_IGN;
  GNUTLS_PK_MAP_ALG_LOOP (ret = p->encipher_type) return ret;

}