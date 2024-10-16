_gnutls_map_pk_get_pk (gnutls_kx_algorithm_t kx_algorithm)
{
  gnutls_pk_algorithm_t ret = -1;

  GNUTLS_PK_MAP_ALG_LOOP (ret = p->pk_algorithm) return ret;
}