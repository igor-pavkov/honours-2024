SSLNetVConnection::sslContextSet(void *ctx)
{
  bool zret = true;
  if (ssl) {
    SSL_set_SSL_CTX(ssl, static_cast<SSL_CTX *>(ctx));
  } else {
    zret = false;
  }
  return zret;
}