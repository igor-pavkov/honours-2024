_gnutls_map_kx_get_cred (gnutls_kx_algorithm_t algorithm, int server)
{
  gnutls_credentials_type_t ret = -1;
  if (server)
    {
      GNUTLS_KX_MAP_LOOP (if (p->algorithm == algorithm) ret =
			  p->server_type);
    }
  else
    {
      GNUTLS_KX_MAP_LOOP (if (p->algorithm == algorithm) ret =
			  p->client_type);
    }

  return ret;
}