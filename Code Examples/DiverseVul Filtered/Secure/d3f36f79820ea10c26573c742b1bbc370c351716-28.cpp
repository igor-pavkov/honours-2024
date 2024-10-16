SSLNetVConnection::read_raw_data()
{
  // read data
  int64_t r          = 0;
  int64_t total_read = 0;
  int64_t rattempted = 0;
  char *buffer       = nullptr;
  int buf_len;
  IOBufferBlock *b = this->handShakeBuffer->first_write_block();

  rattempted = b->write_avail();
  while (rattempted) {
    buffer  = b->_end;
    buf_len = rattempted;
    b       = b->next.get();

    r = socketManager.read(this->con.fd, buffer, buf_len);
    NET_INCREMENT_DYN_STAT(net_calls_to_read_stat);
    total_read += rattempted;

    // last read failed or was incomplete
    if (r != rattempted || !b) {
      break;
    }

    rattempted = b->write_avail();
  }

  // If we have already moved some bytes successfully, adjust total_read to reflect reality
  // If any read succeeded, we should return success
  if (r != rattempted) {
    // If the first read failds, we should return error
    if (r <= 0 && total_read > rattempted) {
      r = total_read - rattempted;
    } else {
      r = total_read - rattempted + r;
    }
  }
  NET_SUM_DYN_STAT(net_read_bytes_stat, r);

  IpMap *pp_ipmap;
  pp_ipmap = SSLConfigParams::proxy_protocol_ipmap;

  if (this->get_is_proxy_protocol()) {
    Debug("proxyprotocol", "[SSLNetVConnection::read_raw_data] proxy protocol is enabled on this port");
    if (pp_ipmap->getCount() > 0) {
      Debug("proxyprotocol",
            "[SSLNetVConnection::read_raw_data] proxy protocol has a configured whitelist of trusted IPs - checking");

      // At this point, using get_remote_addr() will return the ip of the
      // proxy source IP, not the Proxy Protocol client ip. Since we are
      // checking the ip of the actual source of this connection, this is
      // what we want now.
      void *payload = nullptr;
      if (!pp_ipmap->contains(get_remote_addr(), &payload)) {
        Debug("proxyprotocol",
              "[SSLNetVConnection::read_raw_data] proxy protocol src IP is NOT in the configured whitelist of trusted IPs - "
              "closing connection");
        r = -ENOTCONN; // Need a quick close/exit here to refuse the connection!!!!!!!!!
        goto proxy_protocol_bypass;
      } else {
        char new_host[INET6_ADDRSTRLEN];
        Debug("proxyprotocol", "[SSLNetVConnection::read_raw_data] Source IP [%s] is in the trusted whitelist for proxy protocol",
              ats_ip_ntop(this->get_remote_addr(), new_host, sizeof(new_host)));
      }
    } else {
      Debug("proxyprotocol",
            "[SSLNetVConnection::read_raw_data] proxy protocol DOES NOT have a configured whitelist of trusted IPs but "
            "proxy protocol is ernabled on this port - processing all connections");
    }

    if (ssl_has_proxy_v1(this, buffer, &r)) {
      Debug("proxyprotocol", "[SSLNetVConnection::read_raw_data] ssl has proxy_v1 header");
      set_remote_addr(get_proxy_protocol_src_addr());
    } else {
      Debug("proxyprotocol",
            "[SSLNetVConnection::read_raw_data] proxy protocol was enabled, but required header was not present in the "
            "transaction - closing connection");
    }
  } // end of Proxy Protocol processing

proxy_protocol_bypass:

  if (r > 0) {
    this->handShakeBuffer->fill(r);

    char *start              = this->handShakeReader->start();
    char *end                = this->handShakeReader->end();
    this->handShakeBioStored = end - start;

    // Sets up the buffer as a read only bio target
    // Must be reset on each read
    BIO *rbio = BIO_new_mem_buf(start, this->handShakeBioStored);
    BIO_set_mem_eof_return(rbio, -1);
    SSL_set0_rbio(this->ssl, rbio);
  } else {
    this->handShakeBioStored = 0;
  }

  Debug("ssl", "%p read r=%" PRId64 " total=%" PRId64 " bio=%d\n", this, r, total_read, this->handShakeBioStored);

  // check for errors
  if (r <= 0) {
    if (r == -EAGAIN || r == -ENOTCONN) {
      NET_INCREMENT_DYN_STAT(net_calls_to_read_nodata_stat);
    }
  }

  return r;
}