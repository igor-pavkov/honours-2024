_gnutls_version_get (int major, int minor)
{
  int ret = -1;

  GNUTLS_VERSION_LOOP (if ((p->major == major) && (p->minor == minor))
		       ret = p->id);
  return ret;
}