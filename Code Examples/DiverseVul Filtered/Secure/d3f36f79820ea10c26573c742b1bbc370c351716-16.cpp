SSLNetVConnection::computeSSLTrace()
{
  // this has to happen before the handshake or else sni_servername will be nullptr
  bool sni_trace;
  if (ssl) {
    const char *ssl_servername   = SSL_get_servername(ssl, TLSEXT_NAMETYPE_host_name);
    char *wire_trace_server_name = SSLConfigParams::ssl_wire_trace_server_name;
    Debug("ssl", "for wiretrace, ssl_servername=%s, wire_trace_server_name=%s", ssl_servername, wire_trace_server_name);
    sni_trace = ssl_servername && wire_trace_server_name && (0 == strcmp(wire_trace_server_name, ssl_servername));
  } else {
    sni_trace = false;
  }

  // count based on ip only if they set an IP value
  const sockaddr *remote_addr = get_remote_addr();
  bool ip_trace               = false;
  if (SSLConfigParams::ssl_wire_trace_ip) {
    ip_trace = (*SSLConfigParams::ssl_wire_trace_ip == remote_addr);
  }

  // count based on percentage
  int percentage = SSLConfigParams::ssl_wire_trace_percentage;
  int random;
  bool trace;

  // we only generate random numbers as needed (to maintain correct percentage)
  if (SSLConfigParams::ssl_wire_trace_server_name && SSLConfigParams::ssl_wire_trace_ip) {
    random = this_ethread()->generator.random() % 100; // range [0-99]
    trace  = sni_trace && ip_trace && (percentage > random);
  } else if (SSLConfigParams::ssl_wire_trace_server_name) {
    random = this_ethread()->generator.random() % 100; // range [0-99]
    trace  = sni_trace && (percentage > random);
  } else if (SSLConfigParams::ssl_wire_trace_ip) {
    random = this_ethread()->generator.random() % 100; // range [0-99]
    trace  = ip_trace && (percentage > random);
  } else {
    random = this_ethread()->generator.random() % 100; // range [0-99]
    trace  = percentage > random;
  }

  Debug("ssl", "ssl_netvc random=%d, trace=%s", random, trace ? "TRUE" : "FALSE");

  return trace;
}