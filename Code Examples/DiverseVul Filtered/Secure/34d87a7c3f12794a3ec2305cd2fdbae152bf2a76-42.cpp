_gnutls_x509_oid2pk_algorithm (const char *oid)
{
  gnutls_pk_algorithm_t ret = GNUTLS_PK_UNKNOWN;

  GNUTLS_PK_LOOP (if (strcmp (p->oid, oid) == 0)
		  {
		  ret = p->id; break;}
  );
  return ret;
}