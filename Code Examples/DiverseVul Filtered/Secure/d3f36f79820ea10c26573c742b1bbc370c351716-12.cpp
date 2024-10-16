SSLNetVConnection::select_next_protocol(SSL *ssl, const unsigned char **out, unsigned char *outlen,
                                        const unsigned char *in ATS_UNUSED, unsigned inlen ATS_UNUSED, void *)
{
  SSLNetVConnection *netvc = SSLNetVCAccess(ssl);
  const unsigned char *npn = nullptr;
  unsigned npnsz           = 0;

  ink_release_assert(netvc != nullptr);
  if (netvc->npnSet && netvc->npnSet->advertiseProtocols(&npn, &npnsz)) {
    // SSL_select_next_proto chooses the first server-offered protocol that appears in the clients protocol set, ie. the
    // server selects the protocol. This is a n^2 search, so it's preferable to keep the protocol set short.

#if HAVE_SSL_SELECT_NEXT_PROTO
    if (SSL_select_next_proto((unsigned char **)out, outlen, npn, npnsz, in, inlen) == OPENSSL_NPN_NEGOTIATED) {
      Debug("ssl", "selected ALPN protocol %.*s", (int)(*outlen), *out);
      return SSL_TLSEXT_ERR_OK;
    }
#endif /* HAVE_SSL_SELECT_NEXT_PROTO */
  }

  *out    = nullptr;
  *outlen = 0;
  return SSL_TLSEXT_ERR_NOACK;
}