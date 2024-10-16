SSLNetVConnection::advertise_next_protocol(SSL *ssl, const unsigned char **out, unsigned int *outlen, void * /*arg ATS_UNUSED */)
{
  SSLNetVConnection *netvc = SSLNetVCAccess(ssl);

  ink_release_assert(netvc != nullptr);
  if (netvc->npnSet && netvc->npnSet->advertiseProtocols(out, outlen)) {
    // Successful return tells OpenSSL to advertise.
    return SSL_TLSEXT_ERR_OK;
  }

  return SSL_TLSEXT_ERR_NOACK;
}