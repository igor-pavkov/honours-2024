SSLNetVConnection::reenable(NetHandler *nh)
{
  Debug("ssl", "Handshake reenable from state=%d", sslHandshakeHookState);

  switch (sslHandshakeHookState) {
  case HANDSHAKE_HOOKS_PRE_INVOKE:
    sslHandshakeHookState = HANDSHAKE_HOOKS_PRE;
    break;
  case HANDSHAKE_HOOKS_CERT_INVOKE:
    sslHandshakeHookState = HANDSHAKE_HOOKS_CERT;
    break;
  default:
    break;
  }

  // Reenabling from the handshake callback
  //
  // Originally, we would wait for the callback to go again to execute additinonal
  // hooks, but since the callbacks are associated with the context and the context
  // can be replaced by the plugin, it didn't seem reasonable to assume that the
  // callback would be executed again.  So we walk through the rest of the hooks
  // here in the reenable.
  if (curHook != nullptr) {
    curHook = curHook->next();
    Debug("ssl", "iterate from reenable curHook=%p", curHook);
  }
  if (curHook != nullptr) {
    // Invoke the hook and return, wait for next reenable
    if (sslHandshakeHookState == HANDSHAKE_HOOKS_CLIENT_CERT) {
      sslHandshakeHookState = HANDSHAKE_HOOKS_CLIENT_CERT_INVOKE;
      curHook->invoke(TS_EVENT_SSL_VERIFY_CLIENT, this);
    } else if (sslHandshakeHookState == HANDSHAKE_HOOKS_CERT) {
      sslHandshakeHookState = HANDSHAKE_HOOKS_CERT_INVOKE;
      curHook->invoke(TS_EVENT_SSL_CERT, this);
    } else if (sslHandshakeHookState == HANDSHAKE_HOOKS_SNI) {
      curHook->invoke(TS_EVENT_SSL_SERVERNAME, this);
    } else if (sslHandshakeHookState == HANDSHAKE_HOOKS_PRE) {
      Debug("ssl", "Reenable preaccept");
      sslHandshakeHookState = HANDSHAKE_HOOKS_PRE_INVOKE;
      ContWrapper::wrap(nh->mutex.get(), curHook->m_cont, TS_EVENT_VCONN_START, this);
    }
    return;
  } else {
    // Move onto the "next" state
    switch (this->sslHandshakeHookState) {
    case HANDSHAKE_HOOKS_PRE:
    case HANDSHAKE_HOOKS_PRE_INVOKE:
      sslHandshakeHookState = HANDSHAKE_HOOKS_SNI;
      break;
    case HANDSHAKE_HOOKS_SNI:
      sslHandshakeHookState = HANDSHAKE_HOOKS_CERT;
      break;
    case HANDSHAKE_HOOKS_CERT:
    case HANDSHAKE_HOOKS_CERT_INVOKE:
      sslHandshakeHookState = HANDSHAKE_HOOKS_CLIENT_CERT;
      break;
    case HANDSHAKE_HOOKS_CLIENT_CERT:
    case HANDSHAKE_HOOKS_CLIENT_CERT_INVOKE:
      sslHandshakeHookState = HANDSHAKE_HOOKS_DONE;
      break;
    default:
      break;
    }
    Debug("ssl", "iterate from reenable curHook=%p %d", curHook, sslHandshakeHookState);
  }

  this->readReschedule(nh);
}