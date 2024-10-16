SSLNetVConnection::load_buffer_and_write(int64_t towrite, MIOBufferAccessor &buf, int64_t &total_written, int &needs)
{
  int64_t try_to_write;
  int64_t num_really_written       = 0;
  int64_t l                        = 0;
  uint32_t dynamic_tls_record_size = 0;
  ssl_error_t err                  = SSL_ERROR_NONE;

  // Dynamic TLS record sizing
  ink_hrtime now = 0;
  if (SSLConfigParams::ssl_maxrecord == -1) {
    now                       = Thread::get_hrtime_updated();
    int msec_since_last_write = ink_hrtime_diff_msec(now, sslLastWriteTime);

    if (msec_since_last_write > SSL_DEF_TLS_RECORD_MSEC_THRESHOLD) {
      // reset sslTotalBytesSent upon inactivity for SSL_DEF_TLS_RECORD_MSEC_THRESHOLD
      sslTotalBytesSent = 0;
    }
    Debug("ssl", "SSLNetVConnection::loadBufferAndCallWrite, now %" PRId64 ",lastwrite %" PRId64 " ,msec_since_last_write %d", now,
          sslLastWriteTime, msec_since_last_write);
  }

  if (HttpProxyPort::TRANSPORT_BLIND_TUNNEL == this->attributes) {
    return this->super::load_buffer_and_write(towrite, buf, total_written, needs);
  }

  bool trace = getSSLTrace();

  do {
    // What is remaining left in the next block?
    l                   = buf.reader()->block_read_avail();
    char *current_block = buf.reader()->start();

    // check if to amount to write exceeds that in this buffer
    int64_t wavail = towrite - total_written;

    if (l > wavail) {
      l = wavail;
    }

    // TS-2365: If the SSL max record size is set and we have
    // more data than that, break this into smaller write
    // operations.
    //
    // TS-4424: Don't mess with record size if last SSL_write failed with
    // needs write
    if (redoWriteSize) {
      l             = redoWriteSize;
      redoWriteSize = 0;
    } else {
      if (SSLConfigParams::ssl_maxrecord > 0 && l > SSLConfigParams::ssl_maxrecord) {
        l = SSLConfigParams::ssl_maxrecord;
      } else if (SSLConfigParams::ssl_maxrecord == -1) {
        if (sslTotalBytesSent < SSL_DEF_TLS_RECORD_BYTE_THRESHOLD) {
          dynamic_tls_record_size = SSL_DEF_TLS_RECORD_SIZE;
          SSL_INCREMENT_DYN_STAT(ssl_total_dyn_def_tls_record_count);
        } else {
          dynamic_tls_record_size = SSL_MAX_TLS_RECORD_SIZE;
          SSL_INCREMENT_DYN_STAT(ssl_total_dyn_max_tls_record_count);
        }
        if (l > dynamic_tls_record_size) {
          l = dynamic_tls_record_size;
        }
      }
    }

    if (!l) {
      break;
    }

    try_to_write       = l;
    num_really_written = 0;
    Debug("ssl", "SSLNetVConnection::loadBufferAndCallWrite, before SSLWriteBuffer, l=%" PRId64 ", towrite=%" PRId64 ", b=%p", l,
          towrite, current_block);
    err = SSLWriteBuffer(ssl, current_block, l, num_really_written);

    if (!origin_trace) {
      TraceOut((0 < num_really_written && trace), get_remote_addr(), get_remote_port(), "WIRE TRACE\tbytes=%d\n%.*s",
               (int)num_really_written, (int)num_really_written, current_block);
    } else {
      char origin_trace_ip[INET6_ADDRSTRLEN];
      ats_ip_ntop(origin_trace_addr, origin_trace_ip, sizeof(origin_trace_ip));
      TraceOut((0 < num_really_written && trace), get_remote_addr(), get_remote_port(), "CLIENT %s:%d\ttbytes=%d\n%.*s",
               origin_trace_ip, origin_trace_port, (int)num_really_written, (int)num_really_written, current_block);
    }

    // We wrote all that we thought we should
    if (num_really_written > 0) {
      total_written += num_really_written;
      buf.reader()->consume(num_really_written);
    }

    Debug("ssl", "SSLNetVConnection::loadBufferAndCallWrite,Number of bytes written=%" PRId64 " , total=%" PRId64 "",
          num_really_written, total_written);
    NET_INCREMENT_DYN_STAT(net_calls_to_write_stat);
  } while (num_really_written == try_to_write && total_written < towrite);

  if (total_written > 0) {
    sslLastWriteTime = now;
    sslTotalBytesSent += total_written;
  }
  redoWriteSize = 0;
  if (num_really_written > 0) {
    needs |= EVENTIO_WRITE;
  } else {
    switch (err) {
    case SSL_ERROR_NONE:
      Debug("ssl", "SSL_write-SSL_ERROR_NONE");
      break;
    case SSL_ERROR_WANT_READ:
      needs |= EVENTIO_READ;
      num_really_written = -EAGAIN;
      SSL_INCREMENT_DYN_STAT(ssl_error_want_read);
      Debug("ssl.error", "SSL_write-SSL_ERROR_WANT_READ");
      break;
    case SSL_ERROR_WANT_WRITE:
    case SSL_ERROR_WANT_X509_LOOKUP: {
      if (SSL_ERROR_WANT_WRITE == err) {
        SSL_INCREMENT_DYN_STAT(ssl_error_want_write);
        redoWriteSize = l;
      } else if (SSL_ERROR_WANT_X509_LOOKUP == err) {
        SSL_INCREMENT_DYN_STAT(ssl_error_want_x509_lookup);
        TraceOut(trace, get_remote_addr(), get_remote_port(), "Want X509 lookup");
      }

      needs |= EVENTIO_WRITE;
      num_really_written = -EAGAIN;
      Debug("ssl.error", "SSL_write-SSL_ERROR_WANT_WRITE");
      break;
    }
    case SSL_ERROR_SYSCALL:
      TraceOut(trace, get_remote_addr(), get_remote_port(), "Syscall Error: %s", strerror(errno));
      num_really_written = -errno;
      SSL_INCREMENT_DYN_STAT(ssl_error_syscall);
      Debug("ssl.error", "SSL_write-SSL_ERROR_SYSCALL");
      break;
    // end of stream
    case SSL_ERROR_ZERO_RETURN:
      TraceOut(trace, get_remote_addr(), get_remote_port(), "SSL Error: zero return");
      num_really_written = -errno;
      SSL_INCREMENT_DYN_STAT(ssl_error_zero_return);
      Debug("ssl.error", "SSL_write-SSL_ERROR_ZERO_RETURN");
      break;
    case SSL_ERROR_SSL:
    default: {
      char buf[512];
      unsigned long e = ERR_peek_last_error();
      ERR_error_string_n(e, buf, sizeof(buf));
      TraceIn(trace, get_remote_addr(), get_remote_port(), "SSL Error: sslErr=%d, ERR_get_error=%ld (%s) errno=%d", err, e, buf,
              errno);
      // Treat SSL_ERROR_SSL as EPIPE error.
      num_really_written = -EPIPE;
      SSL_CLR_ERR_INCR_DYN_STAT(this, ssl_error_ssl, "SSL_write-SSL_ERROR_SSL errno=%d", errno);
    } break;
    }
  }
  return num_really_written;
}