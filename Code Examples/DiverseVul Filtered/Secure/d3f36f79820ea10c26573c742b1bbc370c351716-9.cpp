SSLNetVConnection::clear()
{
  if (ssl != nullptr) {
    SSL_free(ssl);
    ssl = nullptr;
  }

  sslHandShakeComplete        = false;
  sslHandshakeBeginTime       = 0;
  sslLastWriteTime            = 0;
  sslTotalBytesSent           = 0;
  sslClientRenegotiationAbort = false;
  sslSessionCacheHit          = false;

  curHook              = nullptr;
  hookOpRequested      = SSL_HOOK_OP_DEFAULT;
  npnSet               = nullptr;
  npnEndpoint          = nullptr;
  sslHandShakeComplete = false;
  free_handshake_buffers();
  sslTrace = false;

  super::clear();
}