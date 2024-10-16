_gnutls_x509_oid2sign_algorithm (const char *oid)
{
  gnutls_sign_algorithm_t ret = 0;

  GNUTLS_SIGN_LOOP (if (strcmp (oid, p->oid) == 0)
		    {
		    ret = p->id; break;}
  );

  if (ret == 0)
    {
      _gnutls_x509_log ("Unknown SIGN OID: '%s'\n", oid);
      return GNUTLS_SIGN_UNKNOWN;
    }
  return ret;
}