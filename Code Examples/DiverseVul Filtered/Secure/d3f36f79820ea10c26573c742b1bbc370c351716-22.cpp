SSLNetVConnection::net_read_io(NetHandler *nh, EThread *lthread)
{
  int ret;
  int64_t r     = 0;
  int64_t bytes = 0;
  NetState *s   = &this->read;

  if (HttpProxyPort::TRANSPORT_BLIND_TUNNEL == this->attributes) {
    this->super::net_read_io(nh, lthread);
    return;
  }

  MUTEX_TRY_LOCK_FOR(lock, s->vio.mutex, lthread, s->vio.cont);
  if (!lock.is_locked()) {
    readReschedule(nh);
    return;
  }
  // Got closed by the HttpSessionManager thread during a migration
  // The closed flag should be stable once we get the s->vio.mutex in that case
  // (the global session pool mutex).
  if (this->closed) {
    this->super::net_read_io(nh, lthread);
    return;
  }
  // If the key renegotiation failed it's over, just signal the error and finish.
  if (sslClientRenegotiationAbort == true) {
    this->read.triggered = 0;
    readSignalError(nh, (int)r);
    Debug("ssl", "[SSLNetVConnection::net_read_io] client renegotiation setting read signal error");
    return;
  }

  // If it is not enabled, lower its priority.  This allows
  // a fast connection to speed match a slower connection by
  // shifting down in priority even if it could read.
  if (!s->enabled || s->vio.op != VIO::READ || s->vio.is_disabled()) {
    read_disable(nh, this);
    return;
  }

  MIOBufferAccessor &buf = s->vio.buffer;
  int64_t ntodo          = s->vio.ntodo();
  ink_assert(buf.writer());

  // Continue on if we are still in the handshake
  if (!getSSLHandShakeComplete()) {
    int err;

    if (get_context() == NET_VCONNECTION_OUT) {
      ret = sslStartHandShake(SSL_EVENT_CLIENT, err);
    } else {
      ret = sslStartHandShake(SSL_EVENT_SERVER, err);
    }
    // If we have flipped to blind tunnel, don't read ahead
    if (this->handShakeReader) {
      if (this->attributes == HttpProxyPort::TRANSPORT_BLIND_TUNNEL) {
        // Now in blind tunnel. Set things up to read what is in the buffer
        // Must send the READ_COMPLETE here before considering
        // forwarding on the handshake buffer, so the
        // SSLNextProtocolTrampoline has a chance to do its
        // thing before forwarding the buffers.
        this->readSignalDone(VC_EVENT_READ_COMPLETE, nh);

        // If the handshake isn't set yet, this means the tunnel
        // decision was make in the SNI callback.  We must move
        // the client hello message back into the standard read.vio
        // so it will get forwarded onto the origin server
        if (!this->getSSLHandShakeComplete()) {
          this->sslHandShakeComplete = true;

          // Copy over all data already read in during the SSL_accept
          // (the client hello message)
          NetState *s            = &this->read;
          MIOBufferAccessor &buf = s->vio.buffer;
          int64_t r              = buf.writer()->write(this->handShakeHolder);
          s->vio.nbytes += r;
          s->vio.ndone += r;

          // Clean up the handshake buffers
          this->free_handshake_buffers();

          if (r > 0) {
            // Kick things again, so the data that was copied into the
            // vio.read buffer gets processed
            this->readSignalDone(VC_EVENT_READ_COMPLETE, nh);
          }
        }
        return; // Leave if we are tunneling
      }
    }
    if (ret == EVENT_ERROR) {
      this->read.triggered = 0;
      readSignalError(nh, err);
    } else if (ret == SSL_HANDSHAKE_WANT_READ || ret == SSL_HANDSHAKE_WANT_ACCEPT) {
      if (SSLConfigParams::ssl_handshake_timeout_in > 0) {
        double handshake_time = ((double)(Thread::get_hrtime() - sslHandshakeBeginTime) / 1000000000);
        Debug("ssl", "ssl handshake for vc %p, took %.3f seconds, configured handshake_timer: %d", this, handshake_time,
              SSLConfigParams::ssl_handshake_timeout_in);
        if (handshake_time > SSLConfigParams::ssl_handshake_timeout_in) {
          Debug("ssl", "ssl handshake for vc %p, expired, release the connection", this);
          read.triggered = 0;
          nh->read_ready_list.remove(this);
          readSignalError(nh, VC_EVENT_EOS);
          return;
        }
      }
      // move over to the socket if we haven't already
      if (this->handShakeBuffer) {
        read.triggered = update_rbio(true);
      } else {
        read.triggered = 0;
      }
      if (!read.triggered) {
        nh->read_ready_list.remove(this);
      }
      readReschedule(nh);
    } else if (ret == SSL_HANDSHAKE_WANT_CONNECT || ret == SSL_HANDSHAKE_WANT_WRITE) {
      write.triggered = 0;
      nh->write_ready_list.remove(this);
      writeReschedule(nh);
    } else if (ret == EVENT_DONE) {
      // If this was driven by a zero length read, signal complete when
      // the handshake is complete. Otherwise set up for continuing read
      // operations.
      if (ntodo <= 0) {
        readSignalDone(VC_EVENT_READ_COMPLETE, nh);
      } else {
        read.triggered = 1;
        if (read.enabled) {
          nh->read_ready_list.in_or_enqueue(this);
        }
      }
    } else if (ret == SSL_WAIT_FOR_HOOK || ret == SSL_WAIT_FOR_ASYNC) {
      // avoid readReschedule - done when the plugin calls us back to reenable
    } else {
      readReschedule(nh);
    }
    return;
  }

  // If there is nothing to do or no space available, disable connection
  if (ntodo <= 0 || !buf.writer()->write_avail() || s->vio.is_disabled()) {
    read_disable(nh, this);
    return;
  }

  // At this point we are at the post-handshake SSL processing
  //
  // not sure if this do-while loop is really needed here, please replace
  // this comment if you know
  do {
    ret = ssl_read_from_net(this, lthread, r);
    if (ret == SSL_READ_READY || ret == SSL_READ_ERROR_NONE) {
      bytes += r;
    }
    ink_assert(bytes >= 0);
  } while ((ret == SSL_READ_READY && bytes == 0) || ret == SSL_READ_ERROR_NONE);

  if (bytes > 0) {
    if (ret == SSL_READ_WOULD_BLOCK || ret == SSL_READ_READY) {
      if (readSignalAndUpdate(VC_EVENT_READ_READY) != EVENT_CONT) {
        Debug("ssl", "ssl_read_from_net, readSignal != EVENT_CONT");
        return;
      }
    }
  }

  switch (ret) {
  case SSL_READ_READY:
    readReschedule(nh);
    return;
    break;
  case SSL_WRITE_WOULD_BLOCK:
  case SSL_READ_WOULD_BLOCK:
    if (lock.get_mutex() != s->vio.mutex.get()) {
      Debug("ssl", "ssl_read_from_net, mutex switched");
      if (ret == SSL_READ_WOULD_BLOCK) {
        readReschedule(nh);
      } else {
        writeReschedule(nh);
      }
      return;
    }
    // reset the trigger and remove from the ready queue
    // we will need to be retriggered to read from this socket again
    read.triggered = 0;
    nh->read_ready_list.remove(this);
    Debug("ssl", "read_from_net, read finished - would block");
#if TS_USE_PORT
    if (ret == SSL_READ_WOULD_BLOCK) {
      readReschedule(nh);
    } else {
      writeReschedule(nh);
    }
#endif
    break;

  case SSL_READ_EOS:
    // close the connection if we have SSL_READ_EOS, this is the return value from ssl_read_from_net() if we get an
    // SSL_ERROR_ZERO_RETURN from SSL_get_error()
    // SSL_ERROR_ZERO_RETURN means that the origin server closed the SSL connection
    read.triggered = 0;
    readSignalDone(VC_EVENT_EOS, nh);

    if (bytes > 0) {
      Debug("ssl", "read_from_net, read finished - EOS");
    } else {
      Debug("ssl", "read_from_net, read finished - 0 useful bytes read, bytes used by SSL layer");
    }
    break;
  case SSL_READ_COMPLETE:
    readSignalDone(VC_EVENT_READ_COMPLETE, nh);
    Debug("ssl", "read_from_net, read finished - signal done");
    break;
  case SSL_READ_ERROR:
    this->read.triggered = 0;
    readSignalError(nh, (int)r);
    Debug("ssl", "read_from_net, read finished - read error");
    break;
  }
}