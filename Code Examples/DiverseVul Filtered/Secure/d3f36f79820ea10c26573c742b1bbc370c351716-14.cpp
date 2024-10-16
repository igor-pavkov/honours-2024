SSLNetVConnection::do_io_close(int lerrno)
{
  if (this->ssl != nullptr && sslHandShakeComplete) {
    callHooks(TS_EVENT_VCONN_CLOSE);
    int shutdown_mode = SSL_get_shutdown(ssl);
    Debug("ssl-shutdown", "previous shutdown state 0x%x", shutdown_mode);
    int new_shutdown_mode = shutdown_mode | SSL_RECEIVED_SHUTDOWN;

    if (new_shutdown_mode != shutdown_mode) {
      // We do not need to sit around and wait for the client's close-notify if
      // they have not already sent it.  We will still be standards compliant
      Debug("ssl-shutdown", "new SSL_set_shutdown 0x%x", new_shutdown_mode);
      SSL_set_shutdown(ssl, new_shutdown_mode);
    }

    // If the peer has already sent a FIN, don't bother with the shutdown
    // They will just send us a RST for our troubles
    // This test is not foolproof.  The client's fin could be on the wire
    // at the same time we send the close-notify.  If so, the client will likely
    // send RST anyway
    char c;
    ssize_t x = recv(this->con.fd, &c, 1, MSG_PEEK);
    // x < 0 means error.  x == 0 means fin sent
    bool do_shutdown = (x > 0);
    if (x < 0) {
      do_shutdown = (errno == EAGAIN || errno == EWOULDBLOCK);
    }
    if (do_shutdown) {
      // Send the close-notify
      int ret = SSL_shutdown(ssl);
      Debug("ssl-shutdown", "SSL_shutdown %s", (ret) ? "success" : "failed");
    } else {
      // Request a quiet shutdown to OpenSSL
      SSL_set_quiet_shutdown(ssl, 1);
      SSL_set_shutdown(ssl, SSL_RECEIVED_SHUTDOWN | SSL_SENT_SHUTDOWN);
      Debug("ssl-shutdown", "Enable quiet shutdown");
    }
  }
  // Go on and do the unix socket cleanups
  super::do_io_close(lerrno);
}