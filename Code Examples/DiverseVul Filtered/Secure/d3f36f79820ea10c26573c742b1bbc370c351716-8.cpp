SSLNetVConnection::callHooks(TSEvent eventId)
{
  // Only dealing with the SNI/CERT hook so far.
  ink_assert(eventId == TS_EVENT_SSL_CERT || eventId == TS_EVENT_SSL_SERVERNAME || eventId == TS_EVENT_SSL_VERIFY_SERVER ||
             eventId == TS_EVENT_SSL_VERIFY_CLIENT || eventId == TS_EVENT_VCONN_CLOSE);
  Debug("ssl", "callHooks sslHandshakeHookState=%d", this->sslHandshakeHookState);

  // Move state if it is appropriate
  switch (this->sslHandshakeHookState) {
  case HANDSHAKE_HOOKS_PRE:
    if (eventId == TS_EVENT_SSL_SERVERNAME) {
      this->sslHandshakeHookState = HANDSHAKE_HOOKS_SNI;
    } else if (eventId == TS_EVENT_SSL_CERT) {
      this->sslHandshakeHookState = HANDSHAKE_HOOKS_CERT;
    }
    break;
  case HANDSHAKE_HOOKS_SNI:
    if (eventId == TS_EVENT_SSL_CERT) {
      this->sslHandshakeHookState = HANDSHAKE_HOOKS_CERT;
    }
    break;
  default:
    break;
  }

  // Look for hooks associated with the event
  switch (this->sslHandshakeHookState) {
  case HANDSHAKE_HOOKS_SNI:
    // The server verify event addresses ATS to origin handshake
    // All the other events are for client to ATS
    if (eventId == TS_EVENT_SSL_VERIFY_SERVER) {
      if (!curHook) {
        curHook = ssl_hooks->get(TS_SSL_VERIFY_SERVER_INTERNAL_HOOK);
      }
    } else {
      if (!curHook) {
        curHook = ssl_hooks->get(TS_SSL_SERVERNAME_INTERNAL_HOOK);
      } else {
        curHook = curHook->next();
      }
      if (!curHook) {
        this->sslHandshakeHookState = HANDSHAKE_HOOKS_CERT;
      }
    }
    break;
  case HANDSHAKE_HOOKS_CERT:
  case HANDSHAKE_HOOKS_CERT_INVOKE:
    if (!curHook) {
      curHook = ssl_hooks->get(TS_SSL_CERT_INTERNAL_HOOK);
    } else {
      curHook = curHook->next();
    }
    if (curHook == nullptr) {
      this->sslHandshakeHookState = HANDSHAKE_HOOKS_CLIENT_CERT;
    } else {
      this->sslHandshakeHookState = HANDSHAKE_HOOKS_CERT_INVOKE;
    }
    break;
  case HANDSHAKE_HOOKS_CLIENT_CERT:
  case HANDSHAKE_HOOKS_CLIENT_CERT_INVOKE:
    if (!curHook) {
      curHook = ssl_hooks->get(TS_SSL_VERIFY_CLIENT_INTERNAL_HOOK);
    } else {
      curHook = curHook->next();
    }
  // fallthrough
  case HANDSHAKE_HOOKS_DONE:
    if (eventId == TS_EVENT_VCONN_CLOSE) {
      curHook = ssl_hooks->get(TS_VCONN_CLOSE_INTERNAL_HOOK);
    }
    break;
  default:
    curHook                     = nullptr;
    this->sslHandshakeHookState = HANDSHAKE_HOOKS_DONE;
    return true;
  }

  Debug("ssl", "callHooks iterated to curHook=%p", curHook);

  this->serverName = const_cast<char *>(SSL_get_servername(this->ssl, TLSEXT_NAMETYPE_host_name));
  if (this->serverName) {
    auto *hs = TunnelMap.find(this->serverName);
    if (hs != nullptr) {
      this->SNIMapping = true;
      this->attributes = HttpProxyPort::TRANSPORT_BLIND_TUNNEL;
      return EVENT_DONE;
    }
  }

  if (SSL_HOOK_OP_TUNNEL == hookOpRequested && SNIMapping) {
    this->attributes = HttpProxyPort::TRANSPORT_BLIND_TUNNEL;
    // Don't mark the handshake as complete yet,
    // Will be checking for that flag not being set after
    // we get out of this callback, and then will shuffle
    // over the buffered handshake packets to the O.S.
    // sslHandShakeComplete = 1;
    return EVENT_DONE;
  }

  bool reenabled = true;
  if (curHook != nullptr) {
    curHook->invoke(eventId, this);
    reenabled =
      (this->sslHandshakeHookState != HANDSHAKE_HOOKS_CERT_INVOKE && this->sslHandshakeHookState != HANDSHAKE_HOOKS_PRE_INVOKE);
    Debug("ssl", "Called hook on state=%d reenabled=%d", sslHandshakeHookState, reenabled);
  }

  return reenabled;
}